/**
 * @file Kernel.cpp
 * @brief Kernel initialization and context management
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Display.h>
#include <myOS/kernel/Kernel.h>
#include <myOS/kernel/Logger.h>
#include <myOS/kernel/Panic.h>
#include <myOS/kernel/Timer.h>
#include <myOS/kernel/memory/PhysicalMemory.h>


#include <myOS/Brush.h>
#include <myOS/Color.h>
#include <myOS/Font.h>
#include <myOS/FrameBuffer.h>
#include <myOS/TextRenderer.h>
#include <myOS/types.h>


#include <myOS/arch/boot.h>
#include <myOS/arch/multiboot2.h>


namespace myOS::kernel {

    // ============================================================================
    // GLOBAL KERNEL CONTEXT
    // ============================================================================

    static Kernel g_kernel          = {};
    static bool g_kernelInitialized = false;

    // Static instances (allocated in BSS, no heap needed)
    static Logger g_logger;
    static Timer g_timer;

    Kernel& kernel() {
        if (!g_kernelInitialized) {
            // Can't use KERNEL_PANIC here - display might not be ready
            while (true) { asm volatile("cli; hlt"); }
        }
        return g_kernel;
    }

    bool isKernelInitialized() { return g_kernelInitialized; }

    // ============================================================================
    // KERNEL INITIALIZATION
    // ============================================================================

    bool initKernel(uint32_t magic, uintptr_t multibootAddr) {
        // Store boot info
        g_kernel.multibootMagic = magic;
        g_kernel.multibootAddr  = multibootAddr;

        // ========================================================================
        // PHASE 1: Core services (no display yet)
        // ========================================================================

        // Initialize logger first (serial port)
        g_kernel.logger         = &g_logger;
        g_kernel.logger->init();

        LOG_INFO("myOS kernel starting...");
        LOG_DEBUG("Multiboot magic: 0x%x, info addr: 0x%x", magic, static_cast<uint32_t>(multibootAddr));

        // Initialize timer
        g_kernel.timer = &g_timer;
        g_kernel.timer->init();
        LOG_DEBUG("Timer initialized");

        // ========================================================================
        // PHASE 2: Validate Multiboot2
        // ========================================================================

        if (!myOS::arch::multiboot2::isValid(magic)) {
            LOG_ERROR("Invalid Multiboot2 magic number!");
            return false;
        }
        LOG_INFO("Multiboot2 validated successfully");

        // ========================================================================
        // PHASE 3: Initialize physical memory
        // ========================================================================

        // Get total memory from multiboot
        uint64_t totalMem = myOS::arch::multiboot2::getHighestMemoryAddress(static_cast<uint32_t>(multibootAddr));
        if (totalMem == 0) {
            LOG_ERROR("Could not determine memory size from Multiboot2!");
            return false;
        }

        // Cap at addressable memory for the architecture
        if (totalMem > SIZE_MAX) { totalMem = SIZE_MAX; }

        g_kernel.totalMemory = static_cast<size_t>(totalMem);
        LOG_INFO("Total memory: %u MB", static_cast<uint32_t>(g_kernel.totalMemory / (1024 * 1024)));

        // Initialize physical memory manager
        PhysicalMemory::initialize(getPlacementAddress(), g_kernel.totalMemory);
        g_kernel.freeMemory = PhysicalMemory::getFreeMemory();

        // ========================================================================
        // PHASE 4: Parse framebuffer info
        // ========================================================================

        const auto* fbTag   = myOS::arch::multiboot2::findFramebuffer(multibootAddr);
        if (!fbTag) {
            LOG_ERROR("No framebuffer tag found in Multiboot2 info!");
            return false;
        }

        if (fbTag->framebuffer_type != static_cast<uint8_t>(myOS::arch::multiboot2::FramebufferType::RGB)) {
            LOG_ERROR("Framebuffer is not RGB type (type=%u)", fbTag->framebuffer_type);
            return false;
        }

        // Store framebuffer info
        g_kernel.fbAddress = static_cast<uintptr_t>(fbTag->framebuffer_addr);
        g_kernel.fbWidth   = static_cast<uint16_t>(fbTag->framebuffer_width);
        g_kernel.fbHeight  = static_cast<uint16_t>(fbTag->framebuffer_height);
        g_kernel.fbPitch   = static_cast<uint16_t>(fbTag->framebuffer_pitch);
        g_kernel.fbBpp     = fbTag->framebuffer_bpp;

        LOG_INFO("Framebuffer: %ux%u, %u bpp, pitch=%u, addr=0x%x",
                 g_kernel.fbWidth,
                 g_kernel.fbHeight,
                 g_kernel.fbBpp,
                 g_kernel.fbPitch,
                 static_cast<uint32_t>(g_kernel.fbAddress));

        // ========================================================================
        // PHASE 5: Initialize graphics
        // ========================================================================

        // Allocate back buffer using physical memory manager
        size_t backBufferSize   = g_kernel.fbWidth * g_kernel.fbHeight * sizeof(uint32_t);
        size_t backBufferFrames = (backBufferSize + PAGE_SIZE - 1) / PAGE_SIZE;
        g_kernel.backBuffer     = static_cast<uint32_t*>(PhysicalMemory::allocateFrames(backBufferFrames));

        if (!g_kernel.backBuffer) {
            LOG_ERROR("Failed to allocate back buffer!");
            return false;
        }
        LOG_DEBUG("Back buffer allocated at 0x%x (%u frames)", reinterpret_cast<uintptr_t>(g_kernel.backBuffer), static_cast<uint32_t>(backBufferFrames));

        // Create Display (thread-safe via __cxa_guard_* in libgcc)
        static Display display(g_kernel.fbWidth, g_kernel.fbHeight, g_kernel.fbAddress, g_kernel.fbPitch, g_kernel.fbBpp, g_kernel.backBuffer);
        g_kernel.display     = &display;
        g_kernel.framebuffer = &display.getFrameBuffer();
        LOG_INFO("Display initialized");

        // Register display for panic screen
        registerPanicDisplay(g_kernel.display);

        // Create Brush
        static Brush brush(*g_kernel.framebuffer);
        g_kernel.brush = &brush;

        // Initialize fonts
        Font::initialize();
        LOG_DEBUG("Fonts initialized");

        // Create TextRenderer
        static TextRenderer textRenderer(*g_kernel.framebuffer, Font::Poppins);
        g_kernel.textRenderer = &textRenderer;

        // ========================================================================
        // LOG MEMORY LAYOUT
        // ========================================================================

        LOG_INFO("=== Memory Layout ===");
        LOG_INFO("  Kernel: 0x%x - 0x%x (%u KB)",
                 static_cast<uint32_t>(arch::boot::getKernelStart()),
                 static_cast<uint32_t>(arch::boot::getKernelEnd()),
                 static_cast<uint32_t>(arch::boot::getKernelSize() / 1024));
        LOG_INFO("  Stack:  0x%x - 0x%x (%u KB)",
                 static_cast<uint32_t>(arch::boot::get_stack_bottom()),
                 static_cast<uint32_t>(arch::boot::get_stack_top()),
                 static_cast<uint32_t>(arch::boot::get_stack_size() / 1024));
        LOG_INFO("  .text:  %u KB, .data: %u KB, .bss: %u KB",
                 static_cast<uint32_t>(arch::boot::getTextSize() / 1024),
                 static_cast<uint32_t>(arch::boot::getDataSize() / 1024),
                 static_cast<uint32_t>(arch::boot::getBssSize() / 1024));

        // ========================================================================
        // DONE
        // ========================================================================

        g_kernelInitialized = true;
        LOG_INFO("Kernel initialization complete");

        return true;
    }

    // ============================================================================
    // KERNEL MAIN
    // ============================================================================

    [[noreturn]] void kernelMain() {
        Kernel& k = kernel();

        LOG_INFO("Entering kernel main");

        // Clear screen
        k.brush->fill(Color::DarkerGray);

        // Draw border
        k.brush->drawRect(10, 10, k.fbWidth - 10, k.fbHeight - 10, Color::Gray600);

        // Draw demo shapes
        k.brush->fillRect(20, 60, 120, 120, Color::DarkBlue);
        k.brush->fillRect(130, 60, 230, 120, Color::DarkGreen);
        k.brush->fillRect(240, 60, 340, 120, Color::DarkRed);
        k.brush->fillCircle(400, 90, 30, Color::Yellow);
        k.brush->drawCircle(480, 90, 30, Color::Cyan);
        k.brush->drawLine(520, 60, 620, 120, Color::White);
        k.brush->drawLine(520, 120, 620, 60, Color::White);

        LOG_DEBUG("Shapes drawn");

        // Draw text
        TextRenderer& text = *k.textRenderer;

        text.setPosition(20, 20);
        text.setCursor(0, 0);
        text << Color::White << "Hello from myOS..\n";

        // Display info section
        text.setPosition(20, 140);
        text.setCursor(0, 0);
        text << Color::Gray400 << "Display: " << Color::White << k.fbWidth << "x" << k.fbHeight << Color::Gray400 << " @ " << Color::White << k.fbBpp << " bpp\n"
             << Color::Gray400 << "FB Address: " << Color::White << HEX() << static_cast<uint32_t>(k.fbAddress) << DEC() << "\n";

        // Memory info section
        text.setPosition(20, 200);
        text.setCursor(0, 0);
        text << Color::Yellow << "=== Memory Layout ===\n"
             << Color::Gray400 << "Physical Memory: " << Color::White << static_cast<uint32_t>(PhysicalMemory::getFreeMemory() / (1024 * 1024)) << " MB free / "
             << static_cast<uint32_t>(PhysicalMemory::getTotalMemory() / (1024 * 1024)) << " MB total\n"
             << Color::Gray400 << "Kernel:  " << Color::White << HEX() << static_cast<uint32_t>(arch::boot::getKernelStart()) << " - "
             << static_cast<uint32_t>(arch::boot::getKernelEnd()) << DEC() << Color::Gray400 << " (" << Color::White << static_cast<uint32_t>(arch::boot::getKernelSize() / 1024)
             << " KB" << Color::Gray400 << ")\n"
             << Color::Gray400 << "Stack:   " << Color::White << HEX() << static_cast<uint32_t>(arch::boot::get_stack_bottom()) << " - "
             << static_cast<uint32_t>(arch::boot::get_stack_top()) << DEC() << Color::Gray400 << " (" << Color::White << static_cast<uint32_t>(arch::boot::get_stack_size() / 1024)
             << " KB" << Color::Gray400 << ")\n"
             << Color::Gray400 << "Sections: .text " << Color::White << static_cast<uint32_t>(arch::boot::getTextSize() / 1024) << " KB" << Color::Gray400 << ", .data "
             << Color::White << static_cast<uint32_t>(arch::boot::getDataSize() / 1024) << " KB" << Color::Gray400 << ", .bss " << Color::White
             << static_cast<uint32_t>(arch::boot::getBssSize() / 1024) << " KB\n";

        // Components
        text.setPosition(20, 360);
        text.setCursor(0, 0);
        text << Color::Yellow << "=== Components ===\n"
             << Color::Cyan << "SDK: " << Color::White << "Color, Font, FrameBuffer, Brush, TextRenderer\n"
             << Color::Cyan << "Kernel: " << Color::White << "Display, Logger, Panic, Timer, IO, PhysicalMemory\n"
             << Color::Cyan << "Libc: " << Color::White << "memory, string, stdio, stdlib, ctype, math\n";

        LOG_INFO("Text rendered");

        // Swap buffers
        k.display->swapBuffers();
        LOG_INFO("Frame displayed");

        // Halt forever
        LOG_INFO("Entering halt loop");
        while (true) { asm volatile("cli; hlt"); }
    }

}  // namespace myOS::kernel
