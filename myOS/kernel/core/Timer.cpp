/**
 * @file Timer.cpp
 * @brief Simple busy-wait timer implementation
 * 
 * Uses PIT (Programmable Interval Timer) in polling mode for delays.
 * No interrupts required - just reads the hardware counter directly.
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Timer.h>

namespace myOS::kernel {

void Timer::init() {
    if (initialized_) return;
    
    // Configure PIT Channel 0:
    // - Channel 0 (bits 7-6 = 00)
    // - Access mode: lobyte/hibyte (bits 5-4 = 11)
    // - Mode 2: Rate generator (bits 3-1 = 010)
    // - Binary counting (bit 0 = 0)
    // Command byte: 0b00110100 = 0x34
    io::outb(io::ports::PIT_COMMAND, 0x34);
    
    // Set the reload value to maximum (65535) for longest count period
    // This gives us about 54.9ms per full cycle
    io::outb(io::ports::PIT_CHANNEL0, 0xFF);  // Low byte
    io::outb(io::ports::PIT_CHANNEL0, 0xFF);  // High byte
    
    initialized_ = true;
}

uint16_t Timer::readCounter() {
    // Latch the current count for channel 0
    // Command: 0b00000000 = latch count for channel 0
    io::outb(io::ports::PIT_COMMAND, 0x00);
    
    // Read the 16-bit counter (low byte first, then high byte)
    uint8_t low = io::inb(io::ports::PIT_CHANNEL0);
    uint8_t high = io::inb(io::ports::PIT_CHANNEL0);
    
    return (static_cast<uint16_t>(high) << 8) | low;
}

void Timer::delayTicks(uint32_t ticks) {
    if (!initialized_) init();
    
    // For delays longer than one PIT cycle (65535 ticks ≈ 54.9ms),
    // we need to count multiple rollovers
    while (ticks > 0) {
        uint16_t start = readCounter();
        
        // Wait for counter to wrap or reach target
        if (ticks >= 65536) {
            // Wait for a full cycle
            uint16_t last = start;
            while (true) {
                uint16_t current = readCounter();
                // Detect wrap-around (counter went from low back to high)
                if (current > last && last < 1000) {
                    break;
                }
                last = current;
            }
            ticks -= 65536;
        } else {
            // Wait for partial cycle
            uint32_t target = (start >= ticks) ? (start - ticks) : 0;
            uint16_t last = start;
            
            while (true) {
                uint16_t current = readCounter();
                
                // Check if we've counted enough ticks
                if (start >= ticks) {
                    // No wrap needed
                    if (current <= target) break;
                } else {
                    // Need to handle wrap-around
                    if (current > last && last < 1000) {
                        // Wrapped around
                        uint32_t elapsed = start + (65536 - current);
                        if (elapsed >= ticks) break;
                    }
                }
                last = current;
            }
            ticks = 0;
        }
    }
}

void Timer::delayUs(uint32_t us) {
    if (!initialized_) init();
    
    // Convert microseconds to PIT ticks
    // PIT frequency ≈ 1.193182 MHz, so 1 tick ≈ 0.838 us
    // ticks = us * 1193182 / 1000000 ≈ us * 1193 / 1000
    uint32_t ticks = (us * 1193) / 1000;
    if (ticks == 0) ticks = 1;  // Minimum 1 tick
    
    delayTicks(ticks);
}

void Timer::delayMs(uint32_t ms) {
    if (!initialized_) init();
    
    // Convert milliseconds to PIT ticks
    // 1ms = 1000us ≈ 1193 ticks
    uint32_t ticks = ms * 1193;
    
    delayTicks(ticks);
}

} // namespace myOS::kernel
