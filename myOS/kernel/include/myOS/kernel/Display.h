/**
 * @file Display.h
 * @brief Hardware display driver
 *
 * KERNEL ONLY - manages physical framebuffer memory.
 *
 * The Display class provides a high-level interface for managing the
 * graphics display hardware, wrapping the SDK's FrameBuffer with
 * display metadata (resolution, color depth, pitch).
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <myOS/types.h>
#include <myOS/FrameBuffer.h>

namespace myOS::kernel {

/**
 * @brief Hardware display driver
 *
 * Encapsulates the physical framebuffer along with display properties.
 * This is kernel-only because it maps physical video memory.
 *
 * @example
 *   Display display(1024, 768, fbAddr, pitch, 32, backBuffer);
 *   display.getFrameBuffer().fill(Color::Black);
 *   display.swapBuffers();
 */
class Display {
public:
    /**
     * @brief Construct a display with the given parameters
     *
     * @param width Display width in pixels
     * @param height Display height in pixels
     * @param framebufferAddress Physical address of the front buffer
     * @param pitch Bytes per scanline
     * @param bitsPerPixel Color depth (typically 32 for RGB)
     * @param backBuffer Pointer to allocated back buffer
     */
    Display(uint16_t width, uint16_t height, uintptr_t framebufferAddress,
            uint16_t pitch, uint8_t bitsPerPixel, uint32_t* backBuffer);

    /**
     * @brief Swap front and back buffers
     */
    void swapBuffers();

    [[nodiscard]] uint16_t getWidth() const { return width_; }
    [[nodiscard]] uint16_t getHeight() const { return height_; }
    [[nodiscard]] uint16_t getPitch() const { return pitch_; }
    [[nodiscard]] uint8_t getBitsPerPixel() const { return bitsPerPixel_; }
    [[nodiscard]] uint8_t getBytesPerPixel() const { return bitsPerPixel_ / 8; }
    [[nodiscard]] uint32_t getVideoMemorySize() const { return static_cast<uint32_t>(pitch_) * height_; }

    /**
     * @brief Get reference to the underlying FrameBuffer
     *
     * Use this to pass to SDK components like Brush and TextRenderer.
     */
    FrameBuffer& getFrameBuffer() { return frameBuffer_; }
    [[nodiscard]] const FrameBuffer& getFrameBuffer() const { return frameBuffer_; }

    // Non-copyable
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

private:
    FrameBuffer frameBuffer_;
    uint16_t width_;
    uint16_t height_;
    uint16_t pitch_;
    uint8_t bitsPerPixel_;
};

} // namespace myOS::kernel

