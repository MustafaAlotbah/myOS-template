/**
 * @file Timer.h
 * @brief Simple busy-wait timer using PIT (Programmable Interval Timer)
 * 
 * KERNEL ONLY - Provides delay functions without requiring interrupt setup.
 * 
 * This is a polling-based timer that reads the PIT counter directly.
 * For more advanced timing (non-blocking, callbacks), you'll need
 * to set up GDT, IDT, and PIC for interrupt-based timing.
 * 
 * Usage:
 *   myOS::kernel::Timer timer;
 *   timer.init();
 *   timer.delayMs(1000);  // Wait 1 second
 *   timer.delayUs(500);   // Wait 500 microseconds
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <myOS/kernel/IO.h>

namespace myOS::kernel {

/**
 * @brief PIT-based timer for delays
 */
class Timer {
public:
    // PIT runs at approximately 1.193182 MHz
    static constexpr uint32_t PIT_FREQUENCY = 1193182;
    
    Timer() = default;
    
    /**
     * @brief Initialize the timer
     * 
     * Sets up PIT channel 0 for timing operations.
     * Must be called before using delay functions.
     */
    void init();
    
    /**
     * @brief Check if timer is initialized
     */
    [[nodiscard]] bool isInitialized() const { return initialized_; }
    
    /**
     * @brief Delay for a specified number of milliseconds
     * @param ms Number of milliseconds to wait
     * 
     * @note This is a blocking busy-wait. The CPU will spin until
     *       the specified time has elapsed.
     */
    void delayMs(uint32_t ms);
    
    /**
     * @brief Delay for a specified number of microseconds
     * @param us Number of microseconds to wait
     * 
     * @note Minimum accurate delay is about 1us. Very short delays
     *       may not be precise due to PIT resolution.
     */
    void delayUs(uint32_t us);
    
    /**
     * @brief Delay for a specified number of PIT ticks
     * @param ticks Number of PIT ticks to wait (1 tick ≈ 0.838 us)
     */
    void delayTicks(uint32_t ticks);
    
    /**
     * @brief Read the current PIT channel 0 counter value
     * @return Current 16-bit counter value (counts down from reload value)
     */
    uint16_t readCounter();

private:
    bool initialized_ = false;
};

} // namespace myOS::kernel
