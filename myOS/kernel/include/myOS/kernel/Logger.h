/**
 * @file Logger.h
 * @brief Serial port logging system for kernel debugging
 *
 * KERNEL ONLY - requires I/O port access.
 *
 * Provides printf-style logging over the COM1 serial port (0x3F8).
 * Log output can be captured in QEMU with:
 *   -serial stdio          (print to terminal)
 *   -serial file:kernel.log (save to file)
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>
#include <myOS/kernel/IO.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

#define DEBUG
#define DEBUG_ALL
// #define DEBUG_TRACE

#define LOG_FUNCTION_NAME __FUNCTION__

// ============================================================================
// LOG MACROS
// ============================================================================

#ifdef DEBUG

#define LOG_ERROR(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Error, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)

#define LOG_WARN(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Warn, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)

#define LOG_INFO(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Info, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)

#ifdef DEBUG_ALL
#define LOG_DEBUG(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Debug, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#ifdef DEBUG_TRACE
#define LOG_TRACE(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Trace, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)
#else
#define LOG_TRACE(...) ((void)0)
#endif

#else

#define LOG_ERROR(...) \
    myOS::kernel::log(myOS::kernel::LogLevel::Error, LOG_FUNCTION_NAME, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  ((void)0)
#define LOG_INFO(...)  ((void)0)
#define LOG_DEBUG(...) ((void)0)
#define LOG_TRACE(...) ((void)0)

#endif

// ============================================================================
// LOGGER API
// ============================================================================

namespace myOS::kernel {

enum class LogLevel : uint8_t {
    Error,
    Warn,
    Info,
    Debug,
    Trace
};

/**
 * @brief Serial port logger class
 * 
 * Manages a serial port for logging output.
 */
class Logger {
public:
    Logger() = default;
    
    /**
     * @brief Initialize the serial port for logging
     * @param baudRate Baud rate (default: 115200)
     * @return true if initialization succeeded
     */
    bool init(uint32_t baudRate = 115200);
    
    /**
     * @brief Check if the logger has been initialized
     */
    [[nodiscard]] bool isInitialized() const { return initialized_; }
    
    /**
     * @brief Write a single character
     */
    void putChar(char ch);
    
    /**
     * @brief Write a string
     */
    void putString(const char* str);
    
    /**
     * @brief Printf-style formatted output
     */
    void printf(const char* format, ...);

private:
    bool waitForTxReady(uint32_t timeout = 100000);
    
    io::Port8 dataPort_{io::ports::COM1};
    io::Port8 ierPort_{io::ports::COM1 + 1};
    io::Port8 fcrPort_{io::ports::COM1 + 2};
    io::Port8 lcrPort_{io::ports::COM1 + 3};
    io::Port8 mcrPort_{io::ports::COM1 + 4};
    io::Port8 lsrPort_{io::ports::COM1 + 5};
    
    bool initialized_ = false;
};

// ============================================================================
// GLOBAL LOGGING FUNCTIONS (use LOG_* macros)
// ============================================================================

/**
 * @brief Internal logging function (use LOG_* macros instead)
 */
void log(LogLevel level, const char* function, uint32_t line, const char* format, ...);

/**
 * @brief Write a raw string to the serial port
 */
void logRaw(const char* str);

/**
 * @brief Write a single character to the serial port
 */
void logChar(char ch);

} // namespace myOS::kernel
