/**
 * @file Color.h
 * @brief ARGB Color representation and predefined color palette
 *
 * Part of the myOS SDK - usable by both kernel and userspace.
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <stdint.h>

namespace myOS {

    /**
     * @brief ARGB Color representation
     *
     * Colors are stored in BGRA format (little-endian ARGB) for direct
     * framebuffer compatibility. The union allows both component access
     * and direct 32-bit value access.
     *
     * @example
     *   Color red(255, 0, 0);           // RGB constructor
     *   Color custom(0xFF00FF00);       // Direct ARGB value
     *   uint32_t pixel = red.getValue(); // Get for framebuffer
     */
    class Color {
    public:
        union {
            struct {
                uint8_t blue;   ///< Blue component (0-255)
                uint8_t green;  ///< Green component (0-255)
                uint8_t red;    ///< Red component (0-255)
                uint8_t alpha;  ///< Alpha component (0-255, 255 = opaque)
            };
            uint32_t value;  ///< Combined ARGB value
        };

        /**
         * @brief Construct from RGB components
         */
        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : blue(b), green(g), red(r), alpha(a) {}

        /**
         * @brief Construct from 32-bit ARGB value
         */
        constexpr explicit Color(uint32_t argb) : value(argb) {}

        /**
         * @brief Default constructor (black)
         */
        constexpr Color() : value(0xFF000000) {}

        /**
         * @brief Get the 32-bit ARGB value
         */
        [[nodiscard]] constexpr uint32_t getValue() const { return value; }

        /**
         * @brief Get the 32-bit color value (alias for getValue)
         */
        [[nodiscard]] constexpr uint32_t getColorValue() const { return value; }

        /**
         * @brief Create a Color from RGB components
         * @param r Red component (0-255)
         * @param g Green component (0-255)
         * @param b Blue component (0-255)
         * @param a Alpha component (0-255, default 255 = opaque)
         * @return Color object
         */
        [[nodiscard]] static constexpr Color fromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) { return Color(r, g, b, a); }

        /**
         * @brief Create a Color from a 32-bit ARGB value
         * @param argb Combined ARGB value (0xAARRGGBB)
         * @return Color object
         */
        [[nodiscard]] static constexpr Color fromARGB(uint32_t argb) { return Color(argb); }

        /**
         * @brief Create a Color from a hex RGB value (no alpha)
         * @param rgb Combined RGB value (0xRRGGBB), alpha set to 255
         * @return Color object
         */
        [[nodiscard]] static constexpr Color fromHex(uint32_t rgb) {
            return Color(static_cast<uint8_t>((rgb >> 16) & 0xFF),  // R
                         static_cast<uint8_t>((rgb >> 8) & 0xFF),   // G
                         static_cast<uint8_t>(rgb & 0xFF),          // B
                         255                                        // A
            );
        }

        // ========================================================================
        // GRAYSCALE PALETTE
        // ========================================================================
        static const Color White;
        static const Color Gray100;
        static const Color Gray200;
        static const Color Gray300;
        static const Color Gray400;
        static const Color Gray500;
        static const Color Gray600;
        static const Color Gray700;
        static const Color Gray800;
        static const Color DarkGray;
        static const Color DarkerGray;
        static const Color DarkestGray;
        static const Color Black;

        // ========================================================================
        // PRIMARY COLORS
        // ========================================================================
        static const Color Red;
        static const Color Green;
        static const Color Blue;

        // ========================================================================
        // SECONDARY COLORS
        // ========================================================================
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;

        // ========================================================================
        // DARK VARIANTS
        // ========================================================================
        static const Color DarkRed;
        static const Color DarkGreen;
        static const Color DarkBlue;
        static const Color DarkerRed;

        // ========================================================================
        // LIGHT VARIANTS
        // ========================================================================
        static const Color LightRed;
        static const Color LightGreen;
        static const Color LightBlue;
        static const Color LighterBlue;

        // ========================================================================
        // OTHER COLORS
        // ========================================================================
        static const Color Orange;
        static const Color Purple;
        static const Color Pink;
        static const Color Brown;
        static const Color Teal;
    };

}  // namespace myOS
