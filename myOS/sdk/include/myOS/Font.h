/**
 * @file Font.h
 * @brief Bitmap font and glyph structures
 *
 * Part of the myOS SDK - usable by both kernel and userspace.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>

namespace myOS {

constexpr int MAX_GLYPH_WIDTH = 16;
constexpr int MAX_FONT_CHARS = 128;

/**
 * @brief Single character glyph (bitmap font)
 *
 * Stores a column-based bitmap where each column is a uint32_t
 * allowing up to 32 pixels height per glyph.
 */
struct Glyph {
    uint32_t bitmap[MAX_GLYPH_WIDTH];  ///< Column-based bitmap
    uint8_t width;                      ///< Glyph width in pixels
    uint8_t height;                     ///< Glyph height in pixels
    int8_t offsetX;                     ///< Horizontal offset
    int8_t offsetY;                     ///< Vertical offset (for descenders)
};

/**
 * @brief Bitmap font with ASCII character set
 *
 * Provides glyph storage and access for text rendering.
 * Built-in fonts are initialized via Font::initialize().
 */
class Font {
public:
    /**
     * @brief Construct font with name
     * @param name Font identifier
     */
    explicit Font(const char* name);

    /**
     * @brief Get glyph for character
     * @param ch Character code (ASCII)
     * @return Reference to glyph data
     */
    [[nodiscard]] const Glyph& getGlyph(uint32_t ch) const;

    /**
     * @brief Set glyph for character
     * @param ch Character code
     * @param glyph Glyph data to set
     */
    void setGlyph(uint32_t ch, const Glyph& glyph);

    /**
     * @brief Get font name
     * @return Font name string
     */
    [[nodiscard]] const char* getName() const { return name_; }

    /**
     * @brief Initialize built-in fonts
     *
     * Must be called before using any built-in fonts.
     */
    static void initialize();

    /// Built-in Poppins font
    static Font Poppins;

private:
    const char* name_;
    Glyph glyphs_[MAX_FONT_CHARS];

    static void initializePoppins();
};

} // namespace myOS

