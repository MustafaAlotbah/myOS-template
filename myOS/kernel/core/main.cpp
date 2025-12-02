/**
 * @file main.cpp
 * @brief Kernel entry point for myOS
 *
 * This is the first C++ code executed after the bootloader.
 * It initializes the kernel and hands off to kernelMain.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Kernel.h>
#include <myOS/kernel/Logger.h>
#include <stddef.h>
#include <stdint.h>


// Linker symbols for C++ constructors
extern "C" void (*first_constructor)();
extern "C" void (*last_constructor)();

/**
 * @brief Call all global/static constructors
 */
static void callConstructors() {
    for (void (**ctor)() = &first_constructor; ctor != &last_constructor; ++ctor) {
        // Call each constructor
        (*ctor)();
    }
}

/**
 * @brief Kernel entry point
 *
 * Called from bootloader (multiboot.asm) with Multiboot2 parameters.
 * Note: On x86, the bootloader passes 32-bit values. On x64, these would
 * be 64-bit. We use uintptr_t internally for portability.
 *
 * @param magic Multiboot2 magic number (should be 0x36d76289)
 * @param multibootAddr Physical address of Multiboot2 info structure
 */
extern "C" [[noreturn]] void kernelEntry(uint32_t magic, uintptr_t multibootAddr) {
    // 1. Call global constructors
    callConstructors();

    // 2. Initialize kernel (logger, timer, display, etc.)
    if (!myOS::kernel::initKernel(magic, multibootAddr)) {
        // Initialization failed - halt (logger already printed error)
        while (true) { asm volatile("cli; hlt"); }
    }

    // 3. Run kernel main
    myOS::kernel::kernelMain();
}
