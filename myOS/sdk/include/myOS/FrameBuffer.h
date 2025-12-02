/**
 * @file FrameBuffer.h
 * @brief Double-buffered framebuffer abstraction
 *
 * Part of the myOS SDK - usable by both kernel and userspace.
 *
 * The FrameBuffer class provides a hardware-independent abstraction
 * for pixel-based rendering. It uses double buffering to prevent
 * screen tearing.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <myOS/types.h>
#include <myOS/Color.h>

namespace myOS {

/**
 * @brief Double-buffered framebuffer for graphics rendering
 *
 * Manages front and back buffers for tear-free rendering.
 * Draw operations go to the back buffer, then swapBuffers()
 * copies to the front (visible) buffer.
 *
 * @example
 *   // Kernel provides the buffer pointers
 *   FrameBuffer fb(800, 600, frontPtr, backPtr);
 *   fb.fill(Color::Black);
 *   fb.drawPixel(100, 100, Color::White);
 *   fb.swapBuffers();  // Make visible
 */
class FrameBuffer {
public:
    /**
     * @brief Construct framebuffer with front and back buffers
     * @param width Width in pixels
     * @param height Height in pixels
     * @param frontBuffer Pointer to visible video memory
     * @param backBuffer Pointer to off-screen drawing buffer
     */
    FrameBuffer(uint16_t width, uint16_t height, uint32_t* frontBuffer, uint32_t* backBuffer);

    /**
     * @brief Fill entire buffer with a color
     * @param color Fill color
     */
    void fill(Color color);

    /**
     * @brief Draw a single pixel
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Pixel color
     */
    void drawPixel(uint32_t x, uint32_t y, Color color);

    /**
     * @brief Copy back buffer to front buffer
     *
     * Call this after rendering a complete frame to make it visible.
     */
    void swapBuffers();

    /**
     * @brief Get pointer to back buffer
     * @return Pointer to back buffer pixels
     */
    [[nodiscard]] uint32_t* getBackBuffer() { return backBuffer_; }

    /**
     * @brief Get pointer to back buffer (const)
     * @return Const pointer to back buffer pixels
     */
    [[nodiscard]] const uint32_t* getBackBuffer() const { return backBuffer_; }

    /**
     * @brief Get framebuffer width
     * @return Width in pixels
     */
    [[nodiscard]] uint16_t getWidth() const { return width_; }

    /**
     * @brief Get framebuffer height
     * @return Height in pixels
     */
    [[nodiscard]] uint16_t getHeight() const { return height_; }

    /**
     * @brief Get total buffer size in bytes
     * @return Size in bytes
     */
    [[nodiscard]] uint32_t getSize() const { return width_ * height_ * sizeof(uint32_t); }

    REMOVE_COPY(FrameBuffer);

private:
    uint16_t width_;
    uint16_t height_;
    uint32_t* frontBuffer_;
    uint32_t* backBuffer_;
};

} // namespace myOS

