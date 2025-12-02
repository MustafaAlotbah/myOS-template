/**
 * @file FrameBuffer.cpp
 * @brief FrameBuffer implementation
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/FrameBuffer.h>

namespace myOS {

FrameBuffer::FrameBuffer(uint16_t width, uint16_t height, uint32_t* frontBuffer, uint32_t* backBuffer)
    : width_(width)
    , height_(height)
    , frontBuffer_(frontBuffer)
    , backBuffer_(backBuffer)
{
}

void FrameBuffer::fill(Color color) {
    const uint32_t colorValue = color.getValue();
    const uint32_t pixelCount = static_cast<uint32_t>(width_) * static_cast<uint32_t>(height_);

    for (uint32_t i = 0; i < pixelCount; ++i) {
        backBuffer_[i] = colorValue;
    }
}

void FrameBuffer::drawPixel(uint32_t x, uint32_t y, Color color) {
    if (x >= width_ || y >= height_) {
        return;
    }

    const uint32_t index = x + y * width_;
    backBuffer_[index] = color.getValue();
}

void FrameBuffer::swapBuffers() {
    const uint32_t pixelCount = static_cast<uint32_t>(width_) * static_cast<uint32_t>(height_);

    for (uint32_t i = 0; i < pixelCount; ++i) {
        frontBuffer_[i] = backBuffer_[i];
    }
}

} // namespace myOS

