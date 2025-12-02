/**
 * @file stdlib.h
 * @brief Standard library functions for freestanding environment
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Converts string to signed long integer
 * @param nptr String to convert
 * @param endptr Pointer to store address of first invalid character
 * @param base Numerical base (2-36, or 0 for auto-detection)
 * @return Converted value
 */
long strtol(const char* nptr, char** endptr, int base);

/**
 * @brief Converts string to unsigned long integer
 * @param nptr String to convert
 * @param endptr Pointer to store address of first invalid character
 * @param base Numerical base (2-36, or 0 for auto-detection)
 * @return Converted value
 */
unsigned long strtoul(const char* nptr, char** endptr, int base);

/**
 * @brief Converts string to integer (base 10)
 * @param str String to convert
 * @return Converted value
 */
int atoi(const char* str);

/**
 * @brief Converts integer to string
 * @param num Integer to convert
 * @param str Output buffer
 * @param base Numerical base (2-36)
 * @param upper_case Use uppercase letters for bases > 10
 */
void itoa(int num, char* str, int base, bool upper_case = false);

/**
 * @brief Converts unsigned 32-bit integer to string
 * @param num Integer to convert
 * @param str Output buffer
 * @param base Numerical base (2-36)
 * @param upper_case Use uppercase letters for bases > 10
 */
void uitoa(uint32_t num, char* str, int base, bool upper_case = false);

/**
 * @brief Converts signed 64-bit integer to string
 * @param num Integer to convert
 * @param str Output buffer
 * @param base Numerical base (2-36)
 * @param upper_case Use uppercase letters for bases > 10
 */
void itoa64(int64_t num, char* str, int base, bool upper_case);

/**
 * @brief Converts unsigned 64-bit integer to string
 * @param num Integer to convert
 * @param str Output buffer
 * @param base Numerical base (2-36)
 * @param upper_case Use uppercase letters for bases > 10
 */
void uitoa64(uint64_t num, char* str, int base, bool upper_case);

/**
 * @brief Reverses a string in-place
 * @param str String to reverse
 * @param length Length of string
 */
void reverse(char str[], int length);

/**
 * @brief Converts floating-point number to string
 * @param value Value to convert
 * @param str Output buffer
 * @param precision Number of decimal places
 */
void ftoa(double value, char* str, int precision = 6);

#ifdef __cplusplus
}
#endif

