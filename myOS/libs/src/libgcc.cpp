/**
 * @file libgcc.cpp
 * @brief GCC runtime support functions for 64-bit operations on 32-bit systems
 * 
 * These functions are normally provided by libgcc, but in a freestanding
 * environment we must implement them ourselves.
 * 
 * Required for 64-bit division and modulo operations on i686.
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <stdint.h>

extern "C" {

/**
 * @brief Performs unsigned 64-bit integer division
 * @param dividend The 64-bit unsigned integer to be divided
 * @param divisor The 64-bit unsigned integer by which to divide
 * @return The quotient of the division (0 if divisor is 0)
 */
uint64_t __udivdi3(uint64_t dividend, uint64_t divisor) {
    if (divisor == 0) {
        return 0;  // Division by zero - return 0
    }

    uint64_t quotient = 0;
    uint64_t remainder = 0;

    for (int i = 63; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (dividend >> i) & 1;
        if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= (uint64_t)1 << i;
        }
    }

    return quotient;
}

/**
 * @brief Performs unsigned 64-bit integer modulus operation
 * @param dividend The 64-bit unsigned integer to be divided
 * @param divisor The 64-bit unsigned integer by which to divide
 * @return The remainder of the division (0 if divisor is 0)
 */
uint64_t __umoddi3(uint64_t dividend, uint64_t divisor) {
    if (divisor == 0) {
        return 0;  // Modulus by zero - return 0
    }

    uint64_t remainder = 0;

    for (int i = 63; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (dividend >> i) & 1;
        if (remainder >= divisor) {
            remainder -= divisor;
        }
    }

    return remainder;
}

/**
 * @brief Performs signed 64-bit integer division
 * @param dividend The 64-bit signed integer to be divided
 * @param divisor The 64-bit signed integer by which to divide
 * @return The quotient of the division (0 if divisor is 0)
 */
int64_t __divdi3(int64_t dividend, int64_t divisor) {
    if (divisor == 0) {
        return 0;  // Division by zero - return 0
    }

    bool negative_result = (dividend < 0) ^ (divisor < 0);

    uint64_t abs_dividend = (dividend < 0) ? -dividend : dividend;
    uint64_t abs_divisor = (divisor < 0) ? -divisor : divisor;

    uint64_t abs_quotient = __udivdi3(abs_dividend, abs_divisor);

    return negative_result ? -(int64_t)abs_quotient : (int64_t)abs_quotient;
}

/**
 * @brief Performs signed 64-bit integer modulus operation
 * @param dividend The 64-bit signed integer to be divided
 * @param divisor The 64-bit signed integer by which to divide
 * @return The remainder of the division (0 if divisor is 0)
 */
int64_t __moddi3(int64_t dividend, int64_t divisor) {
    if (divisor == 0) {
        return 0;  // Modulus by zero - return 0
    }

    uint64_t abs_dividend = (dividend < 0) ? -dividend : dividend;
    uint64_t abs_divisor = (divisor < 0) ? -divisor : divisor;

    uint64_t abs_remainder = __umoddi3(abs_dividend, abs_divisor);

    return (dividend < 0) ? -(int64_t)abs_remainder : (int64_t)abs_remainder;
}

/**
 * @brief Performs signed 64-bit division and modulus in one operation
 * @param dividend The 64-bit signed integer to be divided
 * @param divisor The 64-bit signed integer by which to divide
 * @param remainder Pointer to store the remainder
 * @return The quotient of the division
 */
int64_t __divmoddi4(int64_t dividend, int64_t divisor, int64_t* remainder) {
    if (divisor == 0) {
        if (remainder) *remainder = 0;
        return 0;
    }

    int64_t quotient = __divdi3(dividend, divisor);
    if (remainder) {
        *remainder = dividend - (quotient * divisor);
    }

    return quotient;
}

/**
 * @brief Performs unsigned 64-bit division and modulus in one operation
 * @param dividend The 64-bit unsigned integer to be divided
 * @param divisor The 64-bit unsigned integer by which to divide
 * @param remainder Pointer to store the remainder
 * @return The quotient of the division
 */
uint64_t __udivmoddi4(uint64_t dividend, uint64_t divisor, uint64_t* remainder) {
    if (divisor == 0) {
        if (remainder) *remainder = 0;
        return 0;
    }

    uint64_t quotient = __udivdi3(dividend, divisor);
    if (remainder) {
        *remainder = dividend - (quotient * divisor);
    }

    return quotient;
}

// ============================================================================
// C++ ABI: Static Local Variable Guards
// ============================================================================
// These functions provide thread-safe initialization of static local variables.
// The guard object layout (64-bit):
//   Byte 0: 0 = not initialized, 1 = initialized
//   Byte 1: 0 = not being initialized, 1 = initialization in progress
//   Bytes 2-7: reserved

/**
 * @brief Acquire guard for static initialization
 * 
 * Called before initializing a static local variable.
 * Returns 1 if initialization should proceed, 0 if already initialized.
 * 
 * @param guard Pointer to the 64-bit guard object
 * @return 1 if caller should initialize, 0 if already done
 */
int __cxa_guard_acquire(uint64_t* guard) {
    // Check if already initialized (byte 0)
    char* guard_byte = reinterpret_cast<char*>(guard);
    
    if (guard_byte[0] != 0) {
        return 0;  // Already initialized
    }
    
    // Check if another thread is initializing (byte 1)
    // For single-core: simple spinlock
    // For multi-core: would need atomic compare-exchange
    while (guard_byte[1] != 0) {
        // Spin wait - another initializer is in progress
        // In multi-threaded: should use proper synchronization
        asm volatile("pause" ::: "memory");
    }
    
    // Double-check after acquiring
    if (guard_byte[0] != 0) {
        return 0;  // Someone else finished while we waited
    }
    
    // Mark as "being initialized"
    guard_byte[1] = 1;
    return 1;  // Caller should initialize
}

/**
 * @brief Release guard after successful initialization
 * 
 * Called after successfully initializing a static local variable.
 * 
 * @param guard Pointer to the 64-bit guard object
 */
void __cxa_guard_release(uint64_t* guard) {
    char* guard_byte = reinterpret_cast<char*>(guard);
    
    // Mark as initialized
    guard_byte[0] = 1;
    // Clear "being initialized" flag
    guard_byte[1] = 0;
    
    // Memory barrier to ensure visibility
    asm volatile("" ::: "memory");
}

/**
 * @brief Abort guard after failed initialization (exception thrown)
 * 
 * Called if initialization throws an exception.
 * 
 * @param guard Pointer to the 64-bit guard object
 */
void __cxa_guard_abort(uint64_t* guard) {
    char* guard_byte = reinterpret_cast<char*>(guard);
    
    // Clear "being initialized" flag, leave uninitialized
    guard_byte[1] = 0;
    
    // Memory barrier
    asm volatile("" ::: "memory");
}

} // extern "C"

