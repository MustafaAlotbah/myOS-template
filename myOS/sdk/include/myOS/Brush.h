/**
 * @file Brush.h
 * @brief Drawing primitives for 2D graphics
 *
 * Part of the myOS SDK - usable by both kernel and userspace.
 *
 * The Brush class provides drawing operations on a FrameBuffer:
 *   - Points (pixels)
 *   - Lines (horizontal, vertical, arbitrary using Bresenham)
 *   - Rectangles (filled and outline)
 *   - Circles (filled and outline using Midpoint algorithm)
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#include <myOS/Color.h>
#include <myOS/FrameBuffer.h>
#include <myOS/types.h>


namespace myOS {

    /**
     * @brief 2D drawing primitives for a FrameBuffer
     *
     * Provides shape drawing operations with bounds checking.
     * Uses efficient algorithms (Bresenham for lines, Midpoint for circles).
     *
     * @example
     *   Brush brush(frameBuffer);
     *   brush.fill(Color::Black);
     *   brush.drawRect(10, 10, 100, 100, Color::White);
     *   brush.fillCircle(200, 200, 50, Color::Red);
     */
    class Brush {
    public:
        /**
         * @brief Construct a Brush for the given FrameBuffer
         * @param frameBuffer Reference to the target FrameBuffer
         */
        explicit Brush(FrameBuffer& frameBuffer);

        // ========================================================================
        // FILL OPERATIONS
        // ========================================================================

        /**
         * @brief Fill the entire framebuffer with a solid color
         * @param color Fill color
         */
        void fill(Color color);

        /**
         * @brief Clear the framebuffer (fill with black)
         */
        void clear();

        // ========================================================================
        // POINT OPERATIONS
        // ========================================================================

        /**
         * @brief Draw a single pixel
         * @param x X coordinate
         * @param y Y coordinate
         * @param color Pixel color
         */
        void drawPoint(uint32_t x, uint32_t y, Color color);

        // ========================================================================
        // LINE OPERATIONS
        // ========================================================================

        /**
         * @brief Draw a horizontal line
         * @param x1 Starting X coordinate
         * @param x2 Ending X coordinate
         * @param y Y coordinate
         * @param color Line color
         */
        void drawHLine(uint32_t x1, uint32_t x2, uint32_t y, Color color);

        /**
         * @brief Draw a vertical line
         * @param x X coordinate
         * @param y1 Starting Y coordinate
         * @param y2 Ending Y coordinate
         * @param color Line color
         */
        void drawVLine(uint32_t x, uint32_t y1, uint32_t y2, Color color);

        /**
         * @brief Draw a line between two points using Bresenham's algorithm
         * @param x1 Starting X coordinate
         * @param y1 Starting Y coordinate
         * @param x2 Ending X coordinate
         * @param y2 Ending Y coordinate
         * @param color Line color
         */
        void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color);

        // ========================================================================
        // RECTANGLE OPERATIONS
        // ========================================================================

        /**
         * @brief Draw a filled rectangle
         * @param x1 Top-left X coordinate
         * @param y1 Top-left Y coordinate
         * @param x2 Bottom-right X coordinate
         * @param y2 Bottom-right Y coordinate
         * @param color Fill color
         */
        void fillRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color);

        /**
         * @brief Draw a rectangle outline (frame)
         * @param x1 Top-left X coordinate
         * @param y1 Top-left Y coordinate
         * @param x2 Bottom-right X coordinate
         * @param y2 Bottom-right Y coordinate
         * @param color Outline color
         */
        void drawRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color);

        // ========================================================================
        // CIRCLE OPERATIONS
        // ========================================================================

        /**
         * @brief Draw a circle outline using Midpoint Circle Algorithm
         * @param cx Center X coordinate
         * @param cy Center Y coordinate
         * @param radius Circle radius
         * @param color Circle color
         */
        void drawCircle(uint32_t cx, uint32_t cy, uint32_t radius, Color color);

        /**
         * @brief Draw a filled circle using Midpoint Circle Algorithm
         * @param cx Center X coordinate
         * @param cy Center Y coordinate
         * @param radius Circle radius
         * @param color Fill color
         */
        void fillCircle(uint32_t cx, uint32_t cy, uint32_t radius, Color color);

        REMOVE_COPY(Brush);

    private:
        void plotCirclePoints(uint32_t cx, uint32_t cy, int x, int y, Color color);
        void fillCircleScanlines(uint32_t cx, uint32_t cy, int x, int y, Color color);

        FrameBuffer& frameBuffer_;
    };

}  // namespace myOS
