/**
 * @file stdio.h
 * @brief Standard I/O functions for freestanding environment
 * 
 * Provides sprintf, snprintf, vsprintf, vsnprintf, sscanf implementations.
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes formatted output to a string using a va_list
 * @param str Output buffer
 * @param format Format string
 * @param ap Variable argument list
 * @return Number of characters written (excluding null terminator)
 * 
 * Supported format specifiers:
 *   %s  - String (char*)
 *   %c  - Character
 *   %d  - Signed 32-bit integer
 *   %i  - Signed integer (alias for %d)
 *   %u  - Unsigned 32-bit integer
 *   %x  - Hexadecimal lowercase
 *   %X  - Hexadecimal uppercase
 *   %p  - Pointer address
 *   %b  - Binary representation
 *   %f  - Floating point (with optional precision: %.2f)
 *   %ld, %lu - Long variants
 *   %lld, %llu, %llx, %llX - Long long variants
 *   %zu - size_t
 *   %%  - Literal percent sign
 * 
 * Width specifier: %5d pads with zeros
 */
size_t vsprintf(char* str, const char* format, va_list ap);

/**
 * @brief Writes formatted output to a string
 * @param str Output buffer
 * @param format Format string
 * @param ... Variable arguments
 * @return Number of characters written (excluding null terminator)
 */
size_t sprintf(char* str, const char* format, ...);

/**
 * @brief Writes formatted output to a sized buffer using a va_list
 * @param str Output buffer
 * @param size Maximum number of bytes to write (including null terminator)
 * @param format Format string
 * @param ap Variable argument list
 * @return Number of characters that would have been written
 */
size_t vsnprintf(char* str, size_t size, const char* format, va_list ap);

/**
 * @brief Writes formatted output to a sized buffer
 * @param str Output buffer
 * @param size Maximum number of bytes to write (including null terminator)
 * @param format Format string
 * @param ... Variable arguments
 * @return Number of characters that would have been written
 */
size_t snprintf(char* str, size_t size, const char* format, ...);

/**
 * @brief Parses formatted input from a string using a va_list
 * @param str Input string to parse
 * @param format Format string
 * @param args Variable argument list with pointers
 * @return Number of successfully parsed items
 * 
 * Supported format specifiers:
 *   %d  - Signed integer (int*)
 *   %u  - Unsigned integer (unsigned int*)
 *   %x  - Hexadecimal (unsigned int*)
 *   %s  - String (char*) - REQUIRES width specifier for safety: %10s
 *   %c  - Character (char*)
 *   %f  - Floating point (double*)
 *   %p  - Pointer (void**)
 */
size_t vsscanf(const char* str, const char* format, va_list args);

/**
 * @brief Parses formatted input from a string
 * @param str Input string to parse
 * @param format Format string
 * @param ... Pointers to variables to store parsed values
 * @return Number of successfully parsed items
 */
size_t sscanf(const char* str, const char* format, ...);

#ifdef __cplusplus
}
#endif

