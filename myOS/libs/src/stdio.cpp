/**
 * @file stdio.cpp
 * @brief Standard I/O functions implementation
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/memory.h"
#include "../include/ctype.h"

extern "C" {

size_t vsprintf(char* str, const char* format, va_list args) {
    char* out = str;
    char num_str[40];
    
    for (const char* traverse = format; *traverse; traverse++) {
        if (*traverse != '%') {
            *out++ = *traverse;
            continue;
        }
        
        traverse++;  // Move past '%'
        
        if (*traverse == '%') {
            *out++ = '%';
            continue;
        }
        
        // Parse width and precision
        int width = 0;
        int precision = -1;
        
        if (*traverse >= '0' && *traverse <= '9') {
            width = atoi(traverse);
            while (*traverse >= '0' && *traverse <= '9') traverse++;
        }
        
        if (*traverse == '.') {
            traverse++;
            precision = atoi(traverse);
            while (*traverse >= '0' && *traverse <= '9') traverse++;
        }
        
        switch (*traverse) {
            case 's': {  // String
                char* s = va_arg(args, char*);
                if (s) {
                    strcpy(out, s);
                    out += strlen(s);
                }
                break;
            }
            case 'c': {  // Character
                int c = va_arg(args, int);
                *out++ = (char)c;
                break;
            }
            case 'p': {  // Pointer
                uint32_t p = va_arg(args, uint32_t);
                uitoa(p, num_str, 16, false);
                *out++ = '0';
                *out++ = 'x';
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'd':
            case 'i': {  // Signed integer
                int32_t d = va_arg(args, int32_t);
                itoa(d, num_str, 10, false);
                
                // Handle width formatting with zero padding
                size_t len = strlen(num_str);
                if (width > 0 && (size_t)width > len) {
                    memset(out, '0', width - len);
                    out += width - len;
                }
                
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'u': {  // Unsigned integer
                uint32_t u = va_arg(args, uint32_t);
                uitoa(u, num_str, 10, false);
                
                size_t len = strlen(num_str);
                if (width > 0 && (size_t)width > len) {
                    memset(out, '0', width - len);
                    out += width - len;
                }
                
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'x': {  // Hex lowercase
                uint32_t x = va_arg(args, uint32_t);
                uitoa(x, num_str, 16, false);
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'X': {  // Hex uppercase
                uint32_t x = va_arg(args, uint32_t);
                uitoa(x, num_str, 16, true);
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'b': {  // Binary
                uint32_t b = va_arg(args, uint32_t);
                uitoa(b, num_str, 2, false);
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            case 'z': {  // size_t
                if (*(traverse + 1) == 'u') {
                    traverse++;
                    size_t z = va_arg(args, size_t);
                    uitoa64(z, num_str, 10, false);
                    strcpy(out, num_str);
                    out += strlen(num_str);
                } else {
                    *out++ = '%';
                    *out++ = 'z';
                }
                break;
            }
            case 'l': {  // Long variants
                if (*(traverse + 1) == 'd') {
                    traverse++;
                    int64_t ld = va_arg(args, int64_t);
                    itoa64(ld, num_str, 10, false);
                    strcpy(out, num_str);
                    out += strlen(num_str);
                } else if (*(traverse + 1) == 'u') {
                    traverse++;
                    uint64_t lu = va_arg(args, unsigned long);
                    uitoa64(lu, num_str, 10, false);
                    strcpy(out, num_str);
                    out += strlen(num_str);
                } else if (*(traverse + 1) == 'l') {
                    traverse++;
                    if (*(traverse + 1) == 'd') {
                        traverse++;
                        int64_t lld = va_arg(args, int64_t);
                        itoa64(lld, num_str, 10, false);
                        strcpy(out, num_str);
                        out += strlen(num_str);
                    } else if (*(traverse + 1) == 'u') {
                        traverse++;
                        uint64_t llu = va_arg(args, unsigned long long);
                        uitoa64(llu, num_str, 10, false);
                        strcpy(out, num_str);
                        out += strlen(num_str);
                    } else if (*(traverse + 1) == 'x') {
                        traverse++;
                        uint64_t llx = va_arg(args, unsigned long long);
                        uitoa64(llx, num_str, 16, false);
                        strcpy(out, num_str);
                        out += strlen(num_str);
                    } else if (*(traverse + 1) == 'X') {
                        traverse++;
                        uint64_t llX = va_arg(args, unsigned long long);
                        uitoa64(llX, num_str, 16, true);
                        strcpy(out, num_str);
                        out += strlen(num_str);
                    } else {
                        *out++ = '%';
                        *out++ = 'l';
                        *out++ = 'l';
                    }
                } else {
                    *out++ = '%';
                    *out++ = 'l';
                }
                break;
            }
            case 'f': {  // Floating point
                double f = va_arg(args, double);
                if (precision == -1) precision = 6;
                ftoa(f, num_str, precision);
                strcpy(out, num_str);
                out += strlen(num_str);
                break;
            }
            default:
                *out++ = '%';
                *out++ = *traverse;
                break;
        }
    }
    
    *out = '\0';
    return out - str;
}

size_t sprintf(char* str, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t written = vsprintf(str, format, ap);
    va_end(ap);
    return written;
}

size_t vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    char buffer[1024] = {0};
    
    size_t written = vsprintf(buffer, format, ap);
    
    if (written >= size) {
        written = size - 1;
    }
    
    memcpy(str, buffer, written);
    str[written] = '\0';
    
    return written;
}

size_t snprintf(char* str, size_t size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t written = vsnprintf(str, size, format, ap);
    va_end(ap);
    return written;
}

// ============================================================================
// SCANF HELPERS
// ============================================================================

static const char* skip_whitespace(const char* str) {
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) {
        str++;
    }
    return str;
}

static bool parse_int(const char** str_ptr, int* result) {
    const char* str = skip_whitespace(*str_ptr);
    if (!*str) return false;
    
    bool negative = false;
    if (*str == '-') {
        negative = true;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    if (!(*str >= '0' && *str <= '9')) return false;
    
    int value = 0;
    while (*str >= '0' && *str <= '9') {
        value = value * 10 + (*str - '0');
        str++;
    }
    
    *result = negative ? -value : value;
    *str_ptr = str;
    return true;
}

static bool parse_uint(const char** str_ptr, unsigned int* result) {
    const char* str = skip_whitespace(*str_ptr);
    if (!*str || !(*str >= '0' && *str <= '9')) return false;
    
    unsigned int value = 0;
    while (*str >= '0' && *str <= '9') {
        value = value * 10 + (*str - '0');
        str++;
    }
    
    *result = value;
    *str_ptr = str;
    return true;
}

static bool parse_hex(const char** str_ptr, unsigned int* result) {
    const char* str = skip_whitespace(*str_ptr);
    
    // Handle optional "0x" prefix
    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        str += 2;
    }
    
    if (!*str) return false;
    
    auto is_hex_digit = [](char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    };
    
    auto hex_to_int = [](char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return 0;
    };
    
    if (!is_hex_digit(*str)) return false;
    
    unsigned int value = 0;
    while (*str && is_hex_digit(*str)) {
        value = value * 16 + hex_to_int(*str);
        str++;
    }
    
    *result = value;
    *str_ptr = str;
    return true;
}

static bool parse_float(const char** str_ptr, double* result) {
    const char* str = skip_whitespace(*str_ptr);
    if (!*str) return false;
    
    bool negative = false;
    if (*str == '-') {
        negative = true;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    if (!(*str >= '0' && *str <= '9') && *str != '.') return false;
    
    double value = 0.0;
    double decimal_place = 1.0;
    bool seen_decimal = false;
    
    while (*str) {
        if (*str == '.') {
            if (seen_decimal) break;
            seen_decimal = true;
            str++;
        } else if (*str >= '0' && *str <= '9') {
            if (seen_decimal) {
                decimal_place *= 0.1;
                value += (double)(*str - '0') * decimal_place;
            } else {
                value = value * 10.0 + (double)(*str - '0');
            }
            str++;
        } else {
            break;
        }
    }
    
    *result = negative ? -value : value;
    *str_ptr = str;
    return true;
}

size_t vsscanf(const char* str, const char* format, va_list args) {
    if (!str || !format) return 0;
    
    size_t items_assigned = 0;
    const char* input = str;
    
    for (const char* fmt = format; *fmt; fmt++) {
        if (*fmt != '%') {
            // Handle whitespace in format
            if (*fmt == ' ' || *fmt == '\t' || *fmt == '\n') {
                input = skip_whitespace(input);
                continue;
            }
            
            // Literal character must match
            if (!*input || *input != *fmt) return items_assigned;
            input++;
            continue;
        }
        
        fmt++;  // Move past '%'
        
        if (*fmt == '%') {
            if (*input != '%') return items_assigned;
            input++;
            continue;
        }
        
        // Parse width specifier
        int width = -1;
        if (*fmt >= '0' && *fmt <= '9') {
            width = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
        }
        
        input = skip_whitespace(input);
        
        switch (*fmt) {
            case 'd': {
                int* ptr = va_arg(args, int*);
                if (!ptr || !parse_int(&input, ptr)) return items_assigned;
                items_assigned++;
                break;
            }
            case 'u': {
                unsigned int* ptr = va_arg(args, unsigned int*);
                if (!ptr || !parse_uint(&input, ptr)) return items_assigned;
                items_assigned++;
                break;
            }
            case 'x':
            case 'X': {
                unsigned int* ptr = va_arg(args, unsigned int*);
                if (!ptr || !parse_hex(&input, ptr)) return items_assigned;
                items_assigned++;
                break;
            }
            case 'f': {
                double* ptr = va_arg(args, double*);
                if (!ptr || !parse_float(&input, ptr)) return items_assigned;
                items_assigned++;
                break;
            }
            case 's': {
                char* ptr = va_arg(args, char*);
                if (!ptr || width <= 0) return items_assigned;
                
                int chars_read = 0;
                while (*input && chars_read < width - 1 && !isspace(*input)) {
                    *ptr++ = *input++;
                    chars_read++;
                }
                *ptr = '\0';
                
                if (chars_read == 0) return items_assigned;
                items_assigned++;
                break;
            }
            case 'c': {
                char* ptr = va_arg(args, char*);
                if (!ptr || !*input) return items_assigned;
                *ptr = *input++;
                items_assigned++;
                break;
            }
            case 'p': {
                void** ptr = va_arg(args, void**);
                if (!ptr) return items_assigned;
                unsigned int addr = 0;
                if (!parse_hex(&input, &addr)) return items_assigned;
                *ptr = (void*)(uintptr_t)addr;
                items_assigned++;
                break;
            }
            default:
                return items_assigned;
        }
    }
    
    return items_assigned;
}

size_t sscanf(const char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t result = vsscanf(str, format, args);
    va_end(args);
    return result;
}

} // extern "C"

