/**
 * @file Logger.cpp
 * @brief Serial port logging implementation
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Logger.h>
#include <myOS/kernel/IO.h>
#include <myOS/types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

namespace myOS::kernel {

// ============================================================================
// UART CONSTANTS
// ============================================================================

constexpr uint8_t LSR_TX_EMPTY = 0x20;
constexpr uint8_t LCR_DLAB     = 0x80;
constexpr uint8_t LCR_8N1      = 0x03;
constexpr uint8_t FCR_ENABLE   = 0xC7;
constexpr uint8_t MCR_READY    = 0x0B;

// ============================================================================
// GLOBAL LOGGER INSTANCE
// ============================================================================

static Logger* g_logger = nullptr;

// ============================================================================
// LOGGER CLASS IMPLEMENTATION
// ============================================================================

bool Logger::init(uint32_t baudRate) {
    uint16_t divisor = static_cast<uint16_t>(115200 / baudRate);
    if (divisor == 0) divisor = 1;
    
    ierPort_.write(0x00);
    lcrPort_.write(LCR_DLAB);
    dataPort_.write(divisor & 0xFF);
    ierPort_.write((divisor >> 8) & 0xFF);
    lcrPort_.write(LCR_8N1);
    fcrPort_.write(FCR_ENABLE);
    mcrPort_.write(MCR_READY);
    
    initialized_ = true;
    
    // Set as global logger
    g_logger = this;
    
    // Print header
    putString("\r\n");
    putString("================================================================================\r\n");
    putString("                              myOS Kernel Logger\r\n");
    putString("================================================================================\r\n");
    
    return true;
}

bool Logger::waitForTxReady(uint32_t timeout) {
    for (uint32_t i = 0; i < timeout; ++i) {
        if (lsrPort_.read() & LSR_TX_EMPTY) {
            return true;
        }
    }
    return false;
}

void Logger::putChar(char ch) {
    if (!initialized_) return;
    if (waitForTxReady()) {
        dataPort_.write(static_cast<uint8_t>(ch));
    }
}

void Logger::putString(const char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0' && i < 4096; ++i) {
        putChar(str[i]);
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

namespace {

void uintToString(uint32_t value, char* buffer, int base) {
    static const char digits[] = "0123456789abcdef";
    char temp[32];
    int i = 0;
    
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    while (value > 0 && i < 31) {
        temp[i++] = digits[value % base];
        value /= base;
    }
    
    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

void intToString(int32_t value, char* buffer) {
    if (value < 0) {
        *buffer++ = '-';
        value = -value;
    }
    uintToString(static_cast<uint32_t>(value), buffer, 10);
}

const char* getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        default: return "?????";
    }
}

} // anonymous namespace

void Logger::printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char numBuffer[32];
    
    for (size_t i = 0; format[i] != '\0'; ++i) {
        if (format[i] != '%') {
            putChar(format[i]);
            continue;
        }
        
        ++i;
        if (format[i] == '\0') break;
        
        switch (format[i]) {
            case '%':
                putChar('%');
                break;
            case 'c':
                putChar(static_cast<char>(va_arg(args, int)));
                break;
            case 's': {
                const char* str = va_arg(args, const char*);
                putString(str ? str : "(null)");
                break;
            }
            case 'd':
            case 'i': {
                int32_t val = va_arg(args, int32_t);
                intToString(val, numBuffer);
                putString(numBuffer);
                break;
            }
            case 'u': {
                uint32_t val = va_arg(args, uint32_t);
                uintToString(val, numBuffer, 10);
                putString(numBuffer);
                break;
            }
            case 'x':
            case 'X': {
                uint32_t val = va_arg(args, uint32_t);
                uintToString(val, numBuffer, 16);
                putString(numBuffer);
                break;
            }
            case 'p': {
                void* ptr = va_arg(args, void*);
                putString("0x");
                uintToString(reinterpret_cast<uintptr_t>(ptr), numBuffer, 16);
                putString(numBuffer);
                break;
            }
            default:
                putChar('%');
                putChar(format[i]);
                break;
        }
    }
    
    va_end(args);
}

// ============================================================================
// GLOBAL LOGGING FUNCTIONS
// ============================================================================

void log(LogLevel level, const char* function, uint32_t line, const char* format, ...) {
    if (!g_logger || !g_logger->isInitialized()) return;
    
    g_logger->putString(getLevelString(level));
    g_logger->putString(" [");
    g_logger->putString(function);
    g_logger->putChar(':');
    
    char lineBuffer[16];
    uintToString(line, lineBuffer, 10);
    g_logger->putString(lineBuffer);
    
    g_logger->putString("] ");
    
    va_list args;
    va_start(args, format);
    
    char numBuffer[32];
    
    for (size_t i = 0; format[i] != '\0'; ++i) {
        if (format[i] != '%') {
            g_logger->putChar(format[i]);
            continue;
        }
        
        ++i;
        if (format[i] == '\0') break;
        
        switch (format[i]) {
            case '%':
                g_logger->putChar('%');
                break;
            case 'c':
                g_logger->putChar(static_cast<char>(va_arg(args, int)));
                break;
            case 's': {
                const char* str = va_arg(args, const char*);
                g_logger->putString(str ? str : "(null)");
                break;
            }
            case 'd':
            case 'i': {
                int32_t val = va_arg(args, int32_t);
                intToString(val, numBuffer);
                g_logger->putString(numBuffer);
                break;
            }
            case 'u': {
                uint32_t val = va_arg(args, uint32_t);
                uintToString(val, numBuffer, 10);
                g_logger->putString(numBuffer);
                break;
            }
            case 'x':
            case 'X': {
                uint32_t val = va_arg(args, uint32_t);
                uintToString(val, numBuffer, 16);
                g_logger->putString(numBuffer);
                break;
            }
            case 'p': {
                void* ptr = va_arg(args, void*);
                g_logger->putString("0x");
                uintToString(reinterpret_cast<uintptr_t>(ptr), numBuffer, 16);
                g_logger->putString(numBuffer);
                break;
            }
            default:
                g_logger->putChar('%');
                g_logger->putChar(format[i]);
                break;
        }
    }
    
    va_end(args);
    g_logger->putString("\r\n");
}

void logRaw(const char* str) {
    if (g_logger) g_logger->putString(str);
}

void logChar(char ch) {
    if (g_logger) g_logger->putChar(ch);
}

} // namespace myOS::kernel
