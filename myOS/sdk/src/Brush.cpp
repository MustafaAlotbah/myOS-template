/**
 * @file Brush.cpp
 * @brief Brush drawing primitives implementation
 *
 * Implements 2D drawing algorithms:
 *   - Bresenham's line algorithm
 *   - Midpoint circle algorithm
 *   - Optimized horizontal/vertical line drawing
 *
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#include <myOS/Brush.h>

namespace myOS {

    Brush::Brush(FrameBuffer& frameBuffer) : frameBuffer_(frameBuffer) {}

    // ============================================================================
    // FILL OPERATIONS
    // ============================================================================

    void Brush::fill(Color color) { frameBuffer_.fill(color); }

    void Brush::clear() { fill(Color::Black); }

    // ============================================================================
    // POINT OPERATIONS
    // ============================================================================

    void Brush::drawPoint(uint32_t x, uint32_t y, Color color) {
        if (x < frameBuffer_.getWidth() && y < frameBuffer_.getHeight()) { frameBuffer_.drawPixel(x, y, color); }
    }

    // ============================================================================
    // LINE OPERATIONS
    // ============================================================================

    void Brush::drawHLine(uint32_t x1, uint32_t x2, uint32_t y, Color color) {
        uint32_t width  = frameBuffer_.getWidth();
        uint32_t height = frameBuffer_.getHeight();

        if (y >= height) return;

        if (x1 > x2) {
            uint32_t temp = x1;
            x1            = x2;
            x2            = temp;
        }

        if (x1 >= width) return;
        if (x2 >= width) x2 = width - 1;

        uint32_t* buffer    = frameBuffer_.getBackBuffer();
        uint32_t colorValue = color.getColorValue();

        for (uint32_t x = x1; x <= x2; ++x) { buffer[y * width + x] = colorValue; }
    }

    void Brush::drawVLine(uint32_t x, uint32_t y1, uint32_t y2, Color color) {
        uint32_t width  = frameBuffer_.getWidth();
        uint32_t height = frameBuffer_.getHeight();

        if (x >= width) return;

        if (y1 > y2) {
            uint32_t temp = y1;
            y1            = y2;
            y2            = temp;
        }

        if (y1 >= height) return;
        if (y2 >= height) y2 = height - 1;

        uint32_t* buffer    = frameBuffer_.getBackBuffer();
        uint32_t colorValue = color.getColorValue();

        for (uint32_t y = y1; y <= y2; ++y) { buffer[y * width + x] = colorValue; }
    }

    void Brush::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color) {
        if (y1 == y2) {
            drawHLine(x1, x2, y1, color);
            return;
        }
        if (x1 == x2) {
            drawVLine(x1, y1, y2, color);
            return;
        }

        int dx          = static_cast<int>(x2) - static_cast<int>(x1);
        int dy          = static_cast<int>(y2) - static_cast<int>(y1);

        int absDx       = dx < 0 ? -dx : dx;
        int absDy       = dy < 0 ? -dy : dy;

        int sx          = (x1 < x2) ? 1 : -1;
        int sy          = (y1 < y2) ? 1 : -1;

        int err         = absDx - absDy;

        int x           = static_cast<int>(x1);
        int y           = static_cast<int>(y1);
        int endX        = static_cast<int>(x2);
        int endY        = static_cast<int>(y2);

        uint32_t width  = frameBuffer_.getWidth();
        uint32_t height = frameBuffer_.getHeight();

        int maxSteps    = absDx + absDy + 1;

        for (int i = 0; i < maxSteps; ++i) {
            if (x >= 0 && x < static_cast<int>(width) && y >= 0 && y < static_cast<int>(height)) {
                frameBuffer_.drawPixel(static_cast<uint32_t>(x), static_cast<uint32_t>(y), color);
            }

            if (x == endX && y == endY) break;

            int e2 = 2 * err;

            if (e2 > -absDy) {
                err -= absDy;
                x += sx;
            }

            if (e2 < absDx) {
                err += absDx;
                y += sy;
            }
        }
    }

    // ============================================================================
    // RECTANGLE OPERATIONS
    // ============================================================================

    void Brush::fillRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color) {
        uint32_t width  = frameBuffer_.getWidth();
        uint32_t height = frameBuffer_.getHeight();

        if (width == 0 || height == 0) return;

        if (x1 > x2) {
            uint32_t t = x1;
            x1         = x2;
            x2         = t;
        }
        if (y1 > y2) {
            uint32_t t = y1;
            y1         = y2;
            y2         = t;
        }

        if (x1 >= width) return;
        if (y1 >= height) return;
        if (x2 > width) x2 = width;
        if (y2 > height) y2 = height;

        if (x1 >= x2 || y1 >= y2) return;

        uint32_t* buffer    = frameBuffer_.getBackBuffer();
        uint32_t colorValue = color.getColorValue();
        uint32_t rectWidth  = x2 - x1;

        for (uint32_t y = y1; y < y2; ++y) {
            uint32_t* row = buffer + (y * width) + x1;
            for (uint32_t i = 0; i < rectWidth; ++i) { row[i] = colorValue; }
        }
    }

    void Brush::drawRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color) {
        drawHLine(x1, x2, y1, color);
        drawHLine(x1, x2, y2, color);
        drawVLine(x1, y1, y2, color);
        drawVLine(x2, y1, y2, color);
    }

    // ============================================================================
    // CIRCLE OPERATIONS
    // ============================================================================

    void Brush::plotCirclePoints(uint32_t cx, uint32_t cy, int x, int y, Color color) {
        drawPoint(cx + x, cy + y, color);
        drawPoint(cx - x, cy + y, color);
        drawPoint(cx + x, cy - y, color);
        drawPoint(cx - x, cy - y, color);
        drawPoint(cx + y, cy + x, color);
        drawPoint(cx - y, cy + x, color);
        drawPoint(cx + y, cy - x, color);
        drawPoint(cx - y, cy - x, color);
    }

    void Brush::fillCircleScanlines(uint32_t cx, uint32_t cy, int x, int y, Color color) {
        drawHLine(cx - x, cx + x, cy + y, color);
        drawHLine(cx - x, cx + x, cy - y, color);
        drawHLine(cx - y, cx + y, cy + x, color);
        drawHLine(cx - y, cx + y, cy - x, color);
    }

    void Brush::drawCircle(uint32_t cx, uint32_t cy, uint32_t radius, Color color) {
        if (radius == 0) {
            drawPoint(cx, cy, color);
            return;
        }

        int x = 0;
        int y = static_cast<int>(radius);
        int p = 1 - static_cast<int>(radius);

        plotCirclePoints(cx, cy, x, y, color);

        while (x < y) {
            x++;

            if (p < 0) { p += 2 * x + 1; }
            else {
                y--;
                p += 2 * (x - y) + 1;
            }

            plotCirclePoints(cx, cy, x, y, color);
        }
    }

    void Brush::fillCircle(uint32_t cx, uint32_t cy, uint32_t radius, Color color) {
        if (radius == 0) {
            drawPoint(cx, cy, color);
            return;
        }

        int x = 0;
        int y = static_cast<int>(radius);
        int p = 1 - static_cast<int>(radius);

        fillCircleScanlines(cx, cy, x, y, color);

        while (x < y) {
            x++;

            if (p < 0) { p += 2 * x + 1; }
            else {
                y--;
                p += 2 * (x - y) + 1;
            }

            fillCircleScanlines(cx, cy, x, y, color);
        }
    }

}  // namespace myOS
