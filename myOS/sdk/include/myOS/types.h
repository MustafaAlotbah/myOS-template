/**
 * @file types.h
 * @brief Common type definitions for myOS SDK
 *
 * Provides basic types and utility macros used throughout the SDK.
 * Safe for both kernel and userspace code.
 *
 * Architecture-portable types:
 *   - Use `uintptr_t` for addresses/pointers stored as integers
 *   - Use `size_t` for sizes and counts
 *   - Use fixed-width types (uint32_t, etc.) for hardware registers
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

// Freestanding headers only
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Delete copy constructor and copy assignment operator
 */
#define REMOVE_COPY(cls)                                                                                                                                                           \
    cls(const cls&)            = delete;                                                                                                                                           \
    cls& operator=(const cls&) = delete

/**
 * @brief Define default move operations
 */
#define DEFINE_DEFAULT_MOVE(cls)                                                                                                                                                   \
    cls(cls&&) noexcept            = default;                                                                                                                                      \
    cls& operator=(cls&&) noexcept = default

namespace myOS {

    // size_t is provided by <stddef.h> - automatically correct for architecture
    // On x86:   size_t = 32-bit
    // On x64:   size_t = 64-bit
    // No need to alias it - use ::size_t directly

    // Physical address type - for memory management
    // On x86:   32-bit (4GB addressable)
    // On x64:   64-bit (for PAE or long mode)
    using phys_addr_t = uintptr_t;

    // Virtual address type - for pointer arithmetic
    using virt_addr_t = uintptr_t;

}  // namespace myOS
