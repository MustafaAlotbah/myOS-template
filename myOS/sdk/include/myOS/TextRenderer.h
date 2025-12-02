/**
 * @file TextRenderer.h
 * @brief Text rendering on framebuffer
 *
 * Part of the myOS SDK - usable by both kernel and userspace.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <myOS/Color.h>
#include <myOS/Font.h>
#include <myOS/FrameBuffer.h>
#include <myOS/types.h>


// Convenience macros for stream operators
#define HEX() myOS::TextRenderer::NumeralSystem::Hex
#define DEC() myOS::TextRenderer::NumeralSystem::Dec
#define BIN() myOS::TextRenderer::NumeralSystem::Bin
#define SWAP_BUFF() myOS::TextRenderer::Command::SwapBuffers
#define NEWLINE() myOS::TextRenderer::Command::NewLine

namespace myOS {

    /**
     * @brief Text rendering on a FrameBuffer
     *
     * Renders text using bitmap fonts with cursor tracking,
     * line wrapping, color support, and numeral system formatting.
     *
     * @example
     *   TextRenderer text(fb, Font::Poppins);
     *   text.setCursor(10, 10);
     *   text << Color::White << "Hello, World!\n";
     *   text << "Value: " << HEX() << 0xFF << DEC() << "\n";
     *   text << SWAP_BUFF();  // Swap buffers to display
     */
    class TextRenderer {
    public:
        /**
         * @brief Numeral system for number formatting
         */
        enum class NumeralSystem { Dec, Hex, Bin };

        /**
         * @brief Commands that can be streamed
         */
        enum class Command { SwapBuffers, NewLine };

        /**
         * @brief Construct text renderer
         * @param fb Reference to target FrameBuffer
         * @param font Reference to font to use
         */
        TextRenderer(FrameBuffer& fb, Font& font);

        /**
         * @brief Reset cursor to origin (0, 0)
         */
        void reset();

        /**
         * @brief Set cursor position (relative to text area)
         * @param x X coordinate
         * @param y Y coordinate
         */
        void setCursor(int x, int y);

        /**
         * @brief Set text area position (absolute on screen)
         * @param x X coordinate
         * @param y Y coordinate
         */
        void setPosition(uint32_t x, uint32_t y);

        /**
         * @brief Set text area size
         * @param w Width
         * @param h Height
         */
        void setSize(uint32_t w, uint32_t h);

        /**
         * @brief Set the font
         * @param font New font to use
         */
        void setFont(Font& font);

        /**
         * @brief Set text color
         * @param color New text color
         */
        void setColor(Color color);

        /**
         * @brief Set floating point precision
         * @param precision Number of decimal places
         */
        void setPrecision(uint8_t precision);

        // Getters
        [[nodiscard]] int getCursorX() const { return cursorX_; }
        [[nodiscard]] int getCursorY() const { return cursorY_; }
        [[nodiscard]] uint32_t getPositionX() const { return positionX_; }
        [[nodiscard]] uint32_t getPositionY() const { return positionY_; }
        [[nodiscard]] uint32_t getWidth() const { return width_; }
        [[nodiscard]] uint32_t getHeight() const { return height_; }
        [[nodiscard]] Color getColor() const { return textColor_; }

        /**
         * @brief Calculate width of a string in pixels
         * @param str String to measure
         * @return Width in pixels
         */
        uint32_t calculateWidth(const char* str);

        /**
         * @brief Calculate height of current font
         * @return Height in pixels
         */
        uint32_t calculateHeight();

        /**
         * @brief Render a single character
         * @param ch Character to render
         */
        void putChar(char ch);

        /**
         * @brief Render a string
         * @param str Null-terminated string
         */
        void putString(const char* str);

        /**
         * @brief Move to next line
         */
        void newLine();

        // Stream operators - basic types
        TextRenderer& operator<<(char ch);
        TextRenderer& operator<<(const char* str);
        TextRenderer& operator<<(Color color);

        // Stream operators - integers
        TextRenderer& operator<<(int value);
        TextRenderer& operator<<(uint32_t value);
        TextRenderer& operator<<(int64_t value);
        TextRenderer& operator<<(uint64_t value);
        TextRenderer& operator<<(uint16_t value);

        // Stream operators - floating point
        TextRenderer& operator<<(double value);

        // Stream operators - commands
        TextRenderer& operator<<(NumeralSystem system);
        TextRenderer& operator<<(Command command);

        REMOVE_COPY(TextRenderer);

    private:
        void printNumber(uint64_t value, bool negative);

        FrameBuffer& fb_;
        Font* font_;
        Color textColor_;
        NumeralSystem numSystem_;
        uint8_t precision_;

        int cursorX_;
        int cursorY_;
        uint32_t positionX_;
        uint32_t positionY_;
        uint32_t width_;
        uint32_t height_;
        int lineSpacing_;
    };

}  // namespace myOS
