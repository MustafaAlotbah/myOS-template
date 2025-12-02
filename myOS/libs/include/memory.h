/**
 * @file memory.h
 * @brief Memory manipulation functions for freestanding environment
 *
 * Provides memcpy, memset, memcmp, memmove, and memchr implementations.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Searches for the first occurrence of a value in a memory block
 * @param ptr Pointer to the block of memory
 * @param value Value to be located (passed as uint8_t, converted to unsigned char)
 * @param num Number of bytes to be analyzed
 * @return Pointer to the matching byte or NULL if not found
 */
void* memchr(void* ptr, uint8_t value, size_t num);

/**
 * @brief Compares two blocks of memory
 * @param ptr1 Pointer to the first block of memory
 * @param ptr2 Pointer to the second block of memory
 * @param num Number of bytes to compare
 * @return < 0 if ptr1 < ptr2, 0 if equal, > 0 if ptr1 > ptr2
 */
int memcmp(const void* ptr1, const void* ptr2, size_t num);

/**
 * @brief Copies num bytes from source to destination
 * @param destination Pointer to the destination array
 * @param source Pointer to the source of data
 * @param num Number of bytes to copy
 * @return Pointer to destination
 */
void* memcpy(void* destination, const void* source, size_t num);

/**
 * @brief Copies num double words from source to destination (32-bit optimized)
 * @param destination Pointer to the destination array
 * @param source Pointer to the source of data
 * @param num Number of double words to copy
 * @return Pointer to destination
 */
uint32_t* memcpy32(uint32_t* destination, const uint32_t* source, uint32_t num);

/**
 * @brief Fills memory with a constant byte
 * @param ptr Pointer to the block of memory to fill
 * @param value Value to be set (passed as uint8_t, converted to unsigned char)
 * @param num Number of bytes to be set
 * @return Pointer to the memory area ptr
 */
void* memset(void* ptr, uint8_t value, size_t num);

/**
 * @brief Moves num bytes from src to dest (handles overlapping memory)
 * @param dest Pointer to the destination memory area
 * @param src Pointer to the source memory area
 * @param n Number of bytes to move
 * @return Pointer to destination
 */
void* memmove(void* dest, const void* src, size_t n);

#ifdef __cplusplus
}
#endif
