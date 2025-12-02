/**
 * @file IO.h
 * @brief Low-level x86 I/O port access primitives
 *
 * KERNEL ONLY - requires ring 0 privileges.
 *
 * This header provides two complementary interfaces for hardware I/O:
 *
 * 1. **Inline Functions** - For quick, one-off port access
 *    Example: myOS::kernel::io::outb(0x3F8, 'A');
 *
 * 2. **Port Classes** - For encapsulating device ports with zero overhead
 *    Example: myOS::kernel::io::Port8 serial(0x3F8); serial.write('A');
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>

namespace myOS::kernel::io {

// ============================================================================
// INLINE I/O FUNCTIONS
// ============================================================================

/**
 * @brief Write an 8-bit value to an I/O port
 */
inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * @brief Read an 8-bit value from an I/O port
 */
inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/**
 * @brief Write a 16-bit value to an I/O port
 */
inline void outw(uint16_t port, uint16_t value) {
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * @brief Read a 16-bit value from an I/O port
 */
inline uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/**
 * @brief Write a 32-bit value to an I/O port
 */
inline void outl(uint16_t port, uint32_t value) {
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * @brief Read a 32-bit value from an I/O port
 */
inline uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/**
 * @brief Write with timing delay (for slow hardware)
 */
inline void outb_slow(uint16_t port, uint8_t value) {
    asm volatile(
        "outb %0, %1\n\t"
        "jmp 1f\n\t"
        "1: jmp 1f\n\t"
        "1:"
        : : "a"(value), "Nd"(port)
    );
}

/**
 * @brief I/O delay without port access
 */
inline void io_wait() {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

// ============================================================================
// PORT CLASSES
// ============================================================================

/**
 * @brief 8-bit I/O port class
 */
class Port8 {
public:
    constexpr explicit Port8(uint16_t port) : port_(port) {}

    void write(uint8_t value) const { outb(port_, value); }
    [[nodiscard]] uint8_t read() const { return inb(port_); }
    void writeSlow(uint8_t value) const { outb_slow(port_, value); }
    [[nodiscard]] constexpr uint16_t address() const { return port_; }

private:
    uint16_t port_;
};

/**
 * @brief 16-bit I/O port class
 */
class Port16 {
public:
    constexpr explicit Port16(uint16_t port) : port_(port) {}

    void write(uint16_t value) const { outw(port_, value); }
    [[nodiscard]] uint16_t read() const { return inw(port_); }
    [[nodiscard]] constexpr uint16_t address() const { return port_; }

private:
    uint16_t port_;
};

/**
 * @brief 32-bit I/O port class
 */
class Port32 {
public:
    constexpr explicit Port32(uint16_t port) : port_(port) {}

    void write(uint32_t value) const { outl(port_, value); }
    [[nodiscard]] uint32_t read() const { return inl(port_); }
    [[nodiscard]] constexpr uint16_t address() const { return port_; }

private:
    uint16_t port_;
};

// ============================================================================
// COMMON PORT DEFINITIONS
// ============================================================================

namespace ports {
    constexpr uint16_t COM1 = 0x3F8;
    constexpr uint16_t COM2 = 0x2F8;
    constexpr uint16_t COM3 = 0x3E8;
    constexpr uint16_t COM4 = 0x2E8;

    constexpr uint16_t PIC1_COMMAND = 0x20;
    constexpr uint16_t PIC1_DATA    = 0x21;
    constexpr uint16_t PIC2_COMMAND = 0xA0;
    constexpr uint16_t PIC2_DATA    = 0xA1;

    constexpr uint16_t PIT_CHANNEL0 = 0x40;
    constexpr uint16_t PIT_COMMAND  = 0x43;

    constexpr uint16_t PS2_DATA    = 0x60;
    constexpr uint16_t PS2_STATUS  = 0x64;

    constexpr uint16_t CMOS_ADDRESS = 0x70;
    constexpr uint16_t CMOS_DATA    = 0x71;
}

} // namespace myOS::kernel::io

