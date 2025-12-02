/**
 * @file ctype.h
 * @brief Character classification and conversion functions
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if character is alphanumeric (letter or digit)
 */
int isalnum(int c);

/**
 * @brief Checks if character is alphabetic (letter)
 */
int isalpha(int c);

/**
 * @brief Checks if character is blank (' ' or '\t')
 */
int isblank(int c);

/**
 * @brief Checks if character is a control character
 */
int iscntrl(int c);

/**
 * @brief Checks if character is a decimal digit
 */
int isdigit(int c);

/**
 * @brief Checks if character has graphical representation (not space)
 */
int isgraph(int c);

/**
 * @brief Checks if character is lowercase letter
 */
int islower(int c);

/**
 * @brief Checks if character is printable (including space)
 */
int isprint(int c);

/**
 * @brief Checks if character is punctuation
 */
int ispunct(int c);

/**
 * @brief Checks if character is whitespace
 */
int isspace(int c);

/**
 * @brief Checks if character is uppercase letter
 */
int isupper(int c);

/**
 * @brief Checks if character is hexadecimal digit
 */
int isxdigit(int c);

/**
 * @brief Converts character to lowercase
 */
int tolower(int c);

/**
 * @brief Converts character to uppercase
 */
int toupper(int c);

#ifdef __cplusplus
}
#endif

