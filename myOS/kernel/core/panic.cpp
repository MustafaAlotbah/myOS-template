/**
 * @file panic.cpp
 * @brief Kernel panic implementation
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Panic.h>
#include <myOS/kernel/Display.h>
#include <myOS/kernel/Logger.h>
#include <myOS/Color.h>
#include <myOS/Brush.h>
#include <myOS/Font.h>
#include <myOS/TextRenderer.h>

#include <stdarg.h>

// Forward declaration - defined in stdio
extern "C" size_t vsnprintf(char* str, size_t size, const char* format, va_list ap);

namespace {

/**
 * @brief Disable CPU interrupts
 */
inline void disableInterrupts() {
    asm volatile("cli");
}

/**
 * @brief Halt the CPU
 */
[[noreturn]] inline void halt() {
    while (true) {
        asm volatile("hlt");
    }
}

/**
 * @brief Global display pointer (set by kernel during init)
 */
myOS::kernel::Display* g_display = nullptr;

} // anonymous namespace

namespace myOS::kernel {

/**
 * @brief Register the display for panic screen rendering
 * @param display Pointer to the initialized Display object
 */
void registerPanicDisplay(Display* display) {
    g_display = display;
}

[[noreturn]] void panic(const char* format, ...) {
    // 1. Disable interrupts immediately
    disableInterrupts();
    
    // 2. Format the panic message
    char message[512];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    // 3. Log to serial port (always works, even without display)
    LOG_ERROR("=== KERNEL PANIC ===");
    LOG_ERROR(message);
    
    // 4. Display panic screen if display is available
    if (g_display) {
        uint32_t width = g_display->getWidth();
        uint32_t height = g_display->getHeight();
        
        // Get framebuffer for drawing
        FrameBuffer& fb = g_display->getFrameBuffer();
        Brush brush(fb);
        
        // Define panic window dimensions
        constexpr uint16_t margin = 80;
        uint16_t windowX1 = margin;
        uint16_t windowY1 = margin;
        uint16_t windowX2 = width - margin;
        uint16_t windowY2 = height - margin;
        
        // Draw dark background
        brush.fill(Color::DarkerGray);
        
        // Draw panic window
        brush.fillRect(windowX1, windowY1, windowX2, windowY2, Color::fromRGB(30, 30, 35));
        
        // Draw red title bar
        uint16_t titleBarHeight = 28;
        brush.fillRect(windowX1, windowY1, windowX2, windowY1 + titleBarHeight, Color::DarkRed);
        
        // Draw window border
        brush.drawRect(windowX1, windowY1, windowX2, windowY2, Color::Gray600);
        
        // Initialize text renderer
        TextRenderer text(fb, Font::Poppins);
        
        // Draw title in title bar
        text.setPosition(windowX1 + 10, windowY1 + 6);
        text.setCursor(0, 0);
        text << Color::White << "KERNEL PANIC";
        
        // Draw message content area
        text.setPosition(windowX1 + 20, windowY1 + titleBarHeight + 20);
        text.setSize(windowX2 - windowX1 - 40, windowY2 - windowY1 - titleBarHeight - 60);
        text.setCursor(0, 0);
        text << Color::Red << "A fatal error has occurred and the system has been halted.\n\n"
             << Color::Gray400 << "Error message:\n"
             << Color::White << message << "\n";
        
        // Draw footer
        text.setPosition(windowX1 + 20, windowY2 - 30);
        text.setCursor(0, 0);
        text << Color::Gray600 << "Please restart your computer.";
        
        // Swap buffers to display
        g_display->swapBuffers();
    }
    
    // 5. Halt forever
    halt();
}

} // namespace myOS::kernel

