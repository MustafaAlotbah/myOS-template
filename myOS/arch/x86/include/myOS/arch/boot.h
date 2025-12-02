/**
 * @file boot.h
 * @brief Boot-related symbols and stack information
 *
 * Provides access to linker and assembly symbols for memory layout information.
 * These are useful for debugging and understanding the kernel's memory map.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace myOS::arch::boot {

// ============================================================================
// STACK INFORMATION (from multiboot.asm)
// ============================================================================

/**
 * @brief Get the bottom of the kernel stack (lowest address)
 * @return Address of stack bottom (stack limit)
 */
extern "C" uintptr_t get_stack_bottom();

/**
 * @brief Get the top of the kernel stack (highest address, initial ESP)
 * @return Address of stack top (where ESP starts)
 */
extern "C" uintptr_t get_stack_top();

/**
 * @brief Get the size of the kernel stack
 * @return Stack size in bytes
 */
extern "C" size_t get_stack_size();

// ============================================================================
// LINKER SYMBOLS (from x86.ld)
// ============================================================================

// Kernel boundaries
extern "C" char __kernel_start[];
extern "C" char __kernel_end[];

// Section boundaries
extern "C" char __text_start[];
extern "C" char __text_end[];
extern "C" char __data_start[];
extern "C" char __data_end[];
extern "C" char __bss_start[];
extern "C" char __bss_end[];
extern "C" char __rodata_start[];
extern "C" char __rodata_end[];

// Constructor array
extern "C" char __ctors_start[];
extern "C" char __ctors_end[];

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Get kernel start address
 */
inline uintptr_t getKernelStart() {
    return reinterpret_cast<uintptr_t>(__kernel_start);
}

/**
 * @brief Get kernel end address
 */
inline uintptr_t getKernelEnd() {
    return reinterpret_cast<uintptr_t>(__kernel_end);
}

/**
 * @brief Get kernel size in bytes
 */
inline size_t getKernelSize() {
    return getKernelEnd() - getKernelStart();
}

/**
 * @brief Get text section size
 */
inline size_t getTextSize() {
    return reinterpret_cast<uintptr_t>(__text_end) - reinterpret_cast<uintptr_t>(__text_start);
}

/**
 * @brief Get data section size
 */
inline size_t getDataSize() {
    return reinterpret_cast<uintptr_t>(__data_end) - reinterpret_cast<uintptr_t>(__data_start);
}

/**
 * @brief Get BSS section size
 */
inline size_t getBssSize() {
    return reinterpret_cast<uintptr_t>(__bss_end) - reinterpret_cast<uintptr_t>(__bss_start);
}

} // namespace myOS::arch::boot

