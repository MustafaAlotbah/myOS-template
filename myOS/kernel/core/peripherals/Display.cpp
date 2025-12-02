/**
 * @file Display.cpp
 * @brief Display driver implementation
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/kernel/Display.h>

namespace myOS::kernel {

Display::Display(uint16_t width, uint16_t height, uintptr_t framebufferAddress,
                 uint16_t pitch, uint8_t bitsPerPixel, uint32_t* backBuffer)
    : frameBuffer_(width, height, reinterpret_cast<uint32_t*>(framebufferAddress), backBuffer)
    , width_(width)
    , height_(height)
    , pitch_(pitch)
    , bitsPerPixel_(bitsPerPixel)
{
}

void Display::swapBuffers() {
    frameBuffer_.swapBuffers();
}

} // namespace myOS::kernel

