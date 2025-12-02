/**
 * @file Panic.h
 * @brief Kernel panic handling
 * 
 * KERNEL ONLY - Causes the system to halt with an error message.
 * 
 * Usage:
 *   myOS::kernel::panic("Something went wrong!");
 *   myOS::kernel::panic("Error code: %d, address: 0x%X", code, addr);
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

// Forward declaration
namespace myOS::kernel { class Display; }

namespace myOS::kernel {

/**
 * @brief Register the display for panic screen rendering
 * 
 * Must be called after Display is initialized to enable graphical panic screens.
 * If not called, panic will only log to serial port.
 * 
 * @param display Pointer to the initialized Display object
 */
void registerPanicDisplay(Display* display);

/**
 * @brief Causes the system to halt with a formatted panic message
 * 
 * This function:
 * 1. Disables interrupts (prevents further execution)
 * 2. Displays a panic screen with the error message
 * 3. Logs the error to serial port
 * 4. Halts the CPU forever
 * 
 * @param format Printf-style format string
 * @param ... Variable arguments for format string
 * 
 * @note This function never returns
 */
[[noreturn]] void panic(const char* format, ...);

} // namespace myOS::kernel

// Convenience macro with file/line info
#define KERNEL_PANIC(msg, ...) \
    myOS::kernel::panic("[%s:%d] " msg, __FILE__, __LINE__, ##__VA_ARGS__)

