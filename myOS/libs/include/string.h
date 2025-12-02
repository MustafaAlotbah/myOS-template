/**
 * @file string.h
 * @brief String manipulation functions for freestanding environment
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes the length of a string (excluding null terminator)
 * @param str Pointer to the null-terminated string
 * @return Length of the string
 */
size_t strlen(const char* str);

/**
 * @brief Compares two strings
 * @param s1 First string
 * @param s2 Second string
 * @return < 0 if s1 < s2, 0 if equal, > 0 if s1 > s2
 */
int strcmp(const char* s1, const char* s2);

/**
 * @brief Compares up to n characters of two strings
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return < 0 if s1 < s2, 0 if equal, > 0 if s1 > s2
 */
int strncmp(const char* s1, const char* s2, size_t n);

/**
 * @brief Compares two strings (case-insensitive)
 * @param s1 First string
 * @param s2 Second string
 * @return < 0 if s1 < s2, 0 if equal, > 0 if s1 > s2
 */
int strcasecmp(const char* s1, const char* s2);

/**
 * @brief Compares up to n characters of two strings (case-insensitive)
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum number of characters to compare
 * @return < 0 if s1 < s2, 0 if equal, > 0 if s1 > s2
 */
int strncasecmp(const char* s1, const char* s2, size_t n);

/**
 * @brief Copies a string
 * @param dest Destination buffer
 * @param src Source string
 * @return Pointer to dest
 */
char* strcpy(char* dest, const char* src);

/**
 * @brief Copies up to n characters of a string
 * @param dest Destination buffer
 * @param src Source string
 * @param n Maximum number of characters to copy
 * @return Pointer to dest
 */
char* strncpy(char* dest, const char* src, size_t n);

/**
 * @brief Appends a string to another
 * @param dest Destination string (must have enough space)
 * @param src Source string to append
 * @return Pointer to dest
 */
char* strcat(char* dest, const char* src);

/**
 * @brief Finds the first occurrence of a character in a string
 * @param s String to search
 * @param c Character to find
 * @return Pointer to first occurrence or NULL if not found
 */
char* strchr(const char* s, int c);

/**
 * @brief Finds the first occurrence of any character from accept in s
 * @param s String to search
 * @param accept String containing characters to search for
 * @return Pointer to first occurrence or NULL if not found
 */
char* strpbrk(const char* s, const char* accept);

/**
 * @brief Tokenizes a string using delimiters (thread-unsafe)
 * @param s String to tokenize (NULL for subsequent calls)
 * @param delim Delimiter characters
 * @return Pointer to next token or NULL if no more tokens
 */
char* strtok(char* s, const char* delim);

/**
 * @brief Extracts tokens from a string (modifies stringp)
 * @param stringp Address of pointer to string to tokenize
 * @param delim Delimiter characters
 * @return Pointer to extracted token or NULL if no more tokens
 */
char* strsep(char** stringp, const char* delim);

#ifdef __cplusplus
}
#endif

