//
// Created by pusdo on 29/06/2024.
//

#include "MicroRenderer/Core/Renderer.h"
#include <algorithm>
#include <cmath>

namespace MicroRenderer {
    template<typename T, ColorCoding color_coding>
    Renderer<T, color_coding>::Renderer(uint32 width, uint32 height, uint8* buffer) : framebuffer(width, height, buffer) {

    }

    template<typename T, ColorCoding color_coding>
    void Renderer<T, color_coding>::rasterizeLineDDASafe(T x0, T x1, T y0, T y1, const Vector3<T> &color) {
        x0 = std::clamp(x0, 0, framebuffer.getWidth());
        x1 = std::clamp(x1, 0, framebuffer.getWidth());
        y0 = std::clamp(y0, 0, framebuffer.getHeight());
        y1 = std::clamp(y1, 0, framebuffer.getHeight());
        rasterizeLineDDAUnsafe(x0, x1, y0, y1, color);
    }

    template<typename T, ColorCoding color_coding>
    void Renderer<T, color_coding>::rasterizeLineDDAUnsafe(T x0, T x1, T y0, T y1, const Vector3<T> &color) {
        if (x0 > x1)
            std::swap(x0, x1);
        if (y0 > y1)
            std::swap(y0, y1);
        const T dx = x1 - x0;
        const T dy = y1 - y0;
        // All cases of dx = 0, dy = 0, and both dx,dy = 0 are covered.
        if (dx >= dy) {
            // Line moves in x faster than in y.
            const auto x_start = static_cast<uint32>(std::round(x0));
            const auto x_end = static_cast<uint32>(std::round(x1));
            const T y_step = dy / dx;
            T y = y0;
            T y_raster = std::round(y);
            framebuffer.drawPixel(x_start, static_cast<uint32>(y_raster), color);
            for (uint32 x = x_start + 1; x <= x_end; ++x) {
                y += y_step;
                if (y - y_raster >= static_cast<T>(0.5f)) {
                    y_raster += static_cast<T>(1.f);
                    framebuffer.drawNextPixelInXY(color);
                }
                else {
                    framebuffer.drawNextPixelInX(color);
                }
            }
        }
        else {
            // Line moves in x slower than in y.
            const auto y_start = static_cast<uint32>(std::round(y0));
            const auto y_end = static_cast<uint32>(std::round(y1));
            const T x_step = dx / dy;
            T x = x0;
            T x_raster = std::round(x);
            framebuffer.drawPixel(static_cast<uint32>(x_raster), y_start, color);
            for (uint32 y = y_start + 1; y <= y_end; ++y) {
                x += x_step;
                if (x - x_raster >= static_cast<T>(0.5f)) {
                    x_raster += static_cast<T>(1.f);
                    framebuffer.drawNextPixelInXY(color);
                }
                else {
                    framebuffer.drawNextPixelInY(color);
                }
            }
        }
    }
} // namespace MicroRenderer