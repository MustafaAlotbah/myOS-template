/**
 * @file Kernel.h
 * @brief Kernel context and initialization
 * 
 * Central kernel state management. All kernel subsystems are initialized
 * and accessed through this context.
 * 
 * Usage:
 *   myOS::kernel::initKernel(magic, multibootAddr);
 *   auto& k = myOS::kernel::kernel();
 *   k.display->swapBuffers();
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

// Forward declarations to avoid circular includes
namespace myOS {
    class FrameBuffer;
    class Brush;
    class TextRenderer;
}

namespace myOS::kernel {

// Forward declarations
class Display;
class Logger;
class Timer;

/**
 * @brief Kernel context containing all kernel subsystems
 * 
 * This struct holds pointers to all initialized kernel subsystems.
 * Access via kernel() after initKernel() has been called.
 */
struct Kernel {
    // ========================================================================
    // BOOT INFO
    // ========================================================================
    uint32_t multibootMagic;    // Multiboot magic (always 32-bit per spec)
    uintptr_t multibootAddr;    // Multiboot info structure address
    
    // ========================================================================
    // CORE SUBSYSTEMS
    // ========================================================================
    Logger* logger;
    Timer* timer;
    
    // ========================================================================
    // GRAPHICS
    // ========================================================================
    Display* display;
    FrameBuffer* framebuffer;
    Brush* brush;
    TextRenderer* textRenderer;
    
    // ========================================================================
    // FRAMEBUFFER INFO (from multiboot)
    // ========================================================================
    uintptr_t fbAddress;        // Framebuffer physical address
    uint16_t fbWidth;
    uint16_t fbHeight;
    uint16_t fbPitch;
    uint8_t fbBpp;
    
    // ========================================================================
    // MEMORY
    // ========================================================================
    uint32_t* backBuffer;       // Double buffer for graphics
    size_t totalMemory;         // Total physical memory in bytes
    size_t freeMemory;          // Free physical memory in bytes
};

/**
 * @brief Initialize the kernel
 * 
 * This is the main kernel initialization function. It:
 * 1. Initializes logging (serial port)
 * 2. Initializes timer
 * 3. Parses Multiboot2 info
 * 4. Sets up framebuffer and display
 * 5. Initializes fonts
 * 
 * @param magic Multiboot2 magic number
 * @param multibootAddr Multiboot2 info structure address
 * @return true if initialization succeeded
 */
[[nodiscard]] bool initKernel(uint32_t magic, uintptr_t multibootAddr);

/**
 * @brief Get the kernel context
 * 
 * @return Reference to the global kernel context
 * @note Panics if called before initKernel()
 */
[[nodiscard]] Kernel& kernel();

/**
 * @brief Check if kernel is initialized
 */
[[nodiscard]] bool isKernelInitialized();

/**
 * @brief Kernel main loop / demo
 * 
 * Called after kernel initialization to run the main kernel logic.
 * In a real OS, this would start the scheduler. For the template,
 * it demonstrates the graphics capabilities.
 */
[[noreturn]] void kernelMain();

} // namespace myOS::kernel

