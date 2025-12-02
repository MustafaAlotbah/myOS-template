/**
 * @file TextRenderer.cpp
 * @brief TextRenderer implementation
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/TextRenderer.h>

namespace myOS {

TextRenderer::TextRenderer(FrameBuffer& fb, Font& font)
    : fb_(fb)
    , font_(&font)
    , textColor_(Color::White)
    , numSystem_(NumeralSystem::Dec)
    , precision_(3)
    , cursorX_(0)
    , cursorY_(0)
    , positionX_(0)
    , positionY_(0)
    , width_(fb.getWidth())
    , height_(fb.getHeight())
    , lineSpacing_(3)
{
}

void TextRenderer::reset() {
    cursorX_ = 0;
    cursorY_ = 0;
}

void TextRenderer::setCursor(int x, int y) {
    cursorX_ = x;
    cursorY_ = y;
}

void TextRenderer::setPosition(uint32_t x, uint32_t y) {
    positionX_ = x;
    positionY_ = y;
}

void TextRenderer::setSize(uint32_t w, uint32_t h) {
    width_ = w;
    height_ = h;
}

void TextRenderer::setFont(Font& font) {
    font_ = &font;
}

void TextRenderer::setColor(Color color) {
    textColor_ = color;
}

void TextRenderer::setPrecision(uint8_t precision) {
    precision_ = precision;
}

uint32_t TextRenderer::calculateWidth(const char* str) {
    uint32_t width = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n' || str[i] == '\r') continue;
        if (str[i] == '\t') {
            width += font_->getGlyph(' ').width * 4;
            continue;
        }
        width += font_->getGlyph(static_cast<uint32_t>(str[i])).width;
    }
    return width;
}

uint32_t TextRenderer::calculateHeight() {
    return font_->getGlyph('A').height;
}

void TextRenderer::newLine() {
    cursorX_ = 0;
    cursorY_ += font_->getGlyph('A').height + lineSpacing_;
}

void TextRenderer::putChar(char ch) {
    // Handle control characters
    if (ch == '\n') {
        newLine();
        return;
    }
    if (ch == '\r') {
        cursorX_ = 0;
        return;
    }
    if (ch == '\t') {
        cursorX_ += font_->getGlyph(' ').width * 4;
        return;
    }

    const Glyph& glyph = font_->getGlyph(static_cast<uint32_t>(ch));

    // Handle line wrap
    if (cursorX_ + static_cast<int>(glyph.width) >= static_cast<int>(width_)) {
        newLine();
    }

    // Calculate absolute position
    int absX = static_cast<int>(positionX_) + cursorX_;
    int absY = static_cast<int>(positionY_) + cursorY_;

    // Render glyph (column-based bitmap)
    for (uint8_t col = 0; col < glyph.width; ++col) {
        for (uint8_t row = 0; row < glyph.height; ++row) {
            if (glyph.bitmap[col] & (1 << row)) {
                int x = absX + col;
                int y = absY + glyph.offsetY + glyph.height - row;

                if (x >= 0 && x < static_cast<int>(fb_.getWidth()) && 
                    y >= 0 && y < static_cast<int>(fb_.getHeight())) {
                    fb_.drawPixel(static_cast<uint32_t>(x), static_cast<uint32_t>(y), textColor_);
                }
            }
        }
    }

    cursorX_ += glyph.width;
}

void TextRenderer::putString(const char* str) {
    for (int i = 0; str[i] != '\0' && i < 4096; ++i) {
        putChar(str[i]);
    }
}

TextRenderer& TextRenderer::operator<<(char ch) {
    putChar(ch);
    return *this;
}

TextRenderer& TextRenderer::operator<<(const char* str) {
    putString(str);
    return *this;
}

TextRenderer& TextRenderer::operator<<(Color color) {
    setColor(color);
    return *this;
}

void TextRenderer::printNumber(uint64_t value, bool negative) {
    char buffer[65];  // Max for binary 64-bit + sign + null
    int i = 0;
    int base = 10;
    const char* digits = "0123456789abcdef";
    
    switch (numSystem_) {
        case NumeralSystem::Hex:
            base = 16;
            break;
        case NumeralSystem::Bin:
            base = 2;
            break;
        default:
            base = 10;
            break;
    }
    
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value > 0 && i < 64) {
            buffer[i++] = digits[value % base];
            value /= base;
        }
    }
    
    // Add prefix for non-decimal
    if (numSystem_ == NumeralSystem::Hex) {
        putChar('0');
        putChar('x');
    } else if (numSystem_ == NumeralSystem::Bin) {
        putChar('0');
        putChar('b');
    } else if (negative) {
        putChar('-');
    }
    
    // Print in reverse
    while (i > 0) {
        putChar(buffer[--i]);
    }
}

TextRenderer& TextRenderer::operator<<(int value) {
    bool negative = value < 0;
    uint64_t absValue = negative ? static_cast<uint64_t>(-value) : static_cast<uint64_t>(value);
    printNumber(absValue, negative);
    return *this;
}

TextRenderer& TextRenderer::operator<<(uint32_t value) {
    printNumber(static_cast<uint64_t>(value), false);
    return *this;
}

TextRenderer& TextRenderer::operator<<(int64_t value) {
    bool negative = value < 0;
    uint64_t absValue = negative ? static_cast<uint64_t>(-value) : static_cast<uint64_t>(value);
    printNumber(absValue, negative);
    return *this;
}

TextRenderer& TextRenderer::operator<<(uint64_t value) {
    printNumber(value, false);
    return *this;
}

TextRenderer& TextRenderer::operator<<(uint16_t value) {
    printNumber(static_cast<uint64_t>(value), false);
    return *this;
}

TextRenderer& TextRenderer::operator<<(double value) {
    // Handle negative
    if (value < 0) {
        putChar('-');
        value = -value;
    }
    
    // Integer part
    uint64_t intPart = static_cast<uint64_t>(value);
    printNumber(intPart, false);
    
    // Decimal point
    putChar('.');
    
    // Fractional part
    double frac = value - static_cast<double>(intPart);
    for (uint8_t i = 0; i < precision_; ++i) {
        frac *= 10.0;
        int digit = static_cast<int>(frac);
        putChar('0' + digit);
        frac -= digit;
    }
    
    return *this;
}

TextRenderer& TextRenderer::operator<<(NumeralSystem system) {
    numSystem_ = system;
    return *this;
}

TextRenderer& TextRenderer::operator<<(Command command) {
    switch (command) {
        case Command::SwapBuffers:
            fb_.swapBuffers();
            break;
        case Command::NewLine:
            newLine();
            break;
    }
    return *this;
}

} // namespace myOS
