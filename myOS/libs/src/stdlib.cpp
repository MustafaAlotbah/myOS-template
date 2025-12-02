/**
 * @file stdlib.cpp
 * @brief Standard library functions implementation
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include "../include/stdlib.h"
#include "../include/ctype.h"

extern "C" {

long strtol(const char* nptr, char** endptr, int base) {
    long result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (isspace(*nptr)) nptr++;
    
    // Handle sign
    if (*nptr == '-' || *nptr == '+') {
        sign = (*nptr++ == '-') ? -1 : 1;
    }
    
    // Validate base
    if (base == 0 || base == 16) {
        if (*nptr == '0' && (*(nptr + 1) == 'x' || *(nptr + 1) == 'X')) {
            base = 16;
            nptr += 2;
        }
    }
    if (base == 0) {
        if (*nptr == '0') {
            base = 8;
            nptr++;
        } else {
            base = 10;
        }
    }
    
    // Convert number
    unsigned long digit;
    while (*nptr) {
        if (isdigit(*nptr)) {
            digit = *nptr - '0';
        } else if (isalpha(*nptr)) {
            digit = toupper(*nptr) - 'A' + 10;
        } else {
            break;
        }
        
        if (digit >= (unsigned)base) {
            break;
        }
        
        result = result * base + digit;
        nptr++;
    }
    
    // Set endptr
    if (endptr != nullptr) {
        *endptr = (char*)nptr;
    }
    
    return result * sign;
}

unsigned long strtoul(const char* nptr, char** endptr, int base) {
    unsigned long result = 0;
    
    // Skip whitespace
    while (isspace(*nptr)) nptr++;
    
    // Validate base
    if (base == 0 || base == 16) {
        if (*nptr == '0' && (*(nptr + 1) == 'x' || *(nptr + 1) == 'X')) {
            base = 16;
            nptr += 2;
        }
    }
    if (base == 0) {
        if (*nptr == '0') {
            base = 8;
            nptr++;
        } else {
            base = 10;
        }
    }
    
    // Convert number
    unsigned long digit;
    while (*nptr) {
        if (isdigit(*nptr)) {
            digit = *nptr - '0';
        } else if (isalpha(*nptr)) {
            digit = toupper(*nptr) - 'A' + 10;
        } else {
            break;
        }
        
        if (digit >= (unsigned)base) {
            break;
        }
        
        result = result * base + digit;
        nptr++;
    }
    
    // Set endptr
    if (endptr != nullptr) {
        *endptr = (char*)nptr;
    }
    
    return result;
}

int atoi(const char* str) {
    return (int)strtol(str, nullptr, 10);
}

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void itoa(int num, char* str, int base, bool upper_case) {
    int i = 0;
    bool isNegative = false;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    
    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        if (rem > 9) {
            char offset = upper_case ? 'A' - 10 : 'a' - 10;
            str[i++] = rem + offset;
        } else {
            str[i++] = rem + '0';
        }
        num = num / base;
    }
    
    // If number is negative, append '-'
    if (isNegative && base == 10) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    reverse(str, i);
}

void uitoa(uint32_t num, char* str, int base, bool upper_case) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    // Process individual digits
    while (num != 0) {
        uint32_t rem = num % base;
        if (rem > 9) {
            char offset = upper_case ? 'A' - 10 : 'a' - 10;
            str[i++] = rem + offset;
        } else {
            str[i++] = rem + '0';
        }
        num = num / base;
    }
    
    str[i] = '\0';
    reverse(str, i);
}

void itoa64(int64_t num, char* str, int base, bool upper_case) {
    int i = 0;
    bool isNegative = false;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
    
    // Process individual digits
    while (num != 0) {
        int64_t rem = num % base;
        if (rem > 9) {
            char offset = upper_case ? 'A' - 10 : 'a' - 10;
            str[i++] = rem + offset;
        } else {
            str[i++] = rem + '0';
        }
        num = num / base;
    }
    
    // If number is negative, append '-'
    if (isNegative && base == 10) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    reverse(str, i);
}

void uitoa64(uint64_t num, char* str, int base, bool upper_case) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    // Process individual digits
    while (num != 0) {
        uint64_t rem = num % base;
        if (rem > 9) {
            char offset = upper_case ? 'A' - 10 : 'a' - 10;
            str[i++] = rem + offset;
        } else {
            str[i++] = rem + '0';
        }
        num = num / base;
    }
    
    str[i] = '\0';
    reverse(str, i);
}

void ftoa(double value, char* str, int precision) {
    // Handle negative numbers
    if (value < 0) {
        *str++ = '-';
        value = -value;
    }
    
    // Extract integer part
    int64_t intPart = (int64_t)value;
    double fracPart = value - (double)intPart;
    
    // Convert integer part to string
    char intStr[32];
    itoa64(intPart, intStr, 10, false);
    
    // Copy integer part
    int i = 0;
    while (intStr[i]) {
        *str++ = intStr[i++];
    }
    
    if (precision > 0) {
        *str++ = '.';
        
        // Convert fractional part
        for (int p = 0; p < precision; p++) {
            fracPart *= 10;
            int digit = (int)fracPart;
            *str++ = '0' + digit;
            fracPart -= digit;
        }
    }
    
    *str = '\0';
}

} // extern "C"

