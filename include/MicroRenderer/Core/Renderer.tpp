//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include "Renderer.h"
#include <algorithm>
#include <cmath>

namespace MicroRenderer {
    template<typename T, ColorCoding color_coding>
    Renderer<T, color_coding>::Renderer(uint32 width, uint32 height, uint8* buffer) : framebuffer(width, height, buffer) {

    }

    template<typename T, ColorCoding color_coding>
    void Renderer<T, color_coding>::rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T> &color) {
        x0 = std::clamp(x0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y0 = std::clamp(y0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        x1 = std::clamp(x1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y1 = std::clamp(y1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        rasterizeLineDDAUnsafe(x0, y0, x1, y1, color);
    }

    template<typename T, ColorCoding color_coding>
    void Renderer<T, color_coding>::rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T> &color) {
        // All cases of dx = 0, dy = 0, and both dx = 0 and dy = 0 are covered.
        if (y1 < y0) {
            // Always move in increasing y.
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        const T dy = y1 - y0;
        const T dx = x1 - x0;
        const T dx_abs = std::abs(dx);
        if (dx_abs >= dy) {
            // Line moves in x faster than in y.
            const auto x_start = static_cast<uint32>(std::round(x0));
            const auto x_end = static_cast<uint32>(std::round(x1));
            const T y_step = dy / dx_abs;
            T y = y0;
            T y_raster = std::round(y);
            framebuffer.setCursor(x_start, static_cast<uint32>(y_raster));
            if (dx_abs == dx) {
                // Line moves to the right.
                for (uint32 x = x_start + 1; x <= x_end; ++x) {
                    y += y_step;
                    if (y - y_raster >= static_cast<T>(0.5)) {
                        y_raster += static_cast<T>(1.0);
                        framebuffer.moveCursorDown();
                    }
                    framebuffer.moveCursorRight();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
            else {
                // Line moves to the left.
                for (uint32 x = x_start - 1; x >= x_end; --x) {
                    y += y_step;
                    if (y - y_raster >= static_cast<T>(0.5)) {
                        y_raster += static_cast<T>(1.0);
                        framebuffer.moveCursorDown();
                    }
                    framebuffer.moveCursorLeft();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
        }
        else {
            // Line moves in x slower than in y.
            const auto y_start = static_cast<uint32>(std::round(y0));
            const auto y_end = static_cast<uint32>(std::round(y1));
            const T x_step = dx_abs / dy;
            T x = x0;
            T x_raster = std::round(x);
            framebuffer.setCursor(static_cast<uint32>(x_raster), y_start);
            if (dx == dx_abs) {
                // Line moves to the right.
                for (uint32 y = y_start + 1; y <= y_end; ++y) {
                    x += x_step;
                    if (x - x_raster >= static_cast<T>(0.5)) {
                        x_raster += static_cast<T>(1.0);
                        framebuffer.moveCursorRight();
                    }
                    framebuffer.moveCursorDown();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
            else {
                // Line moves to the left.
                for (uint32 y = y_start + 1; y <= y_end; ++y) {
                    x -= x_step;
                    if (x_raster - x >= static_cast<T>(0.5)) {
                        x_raster -= static_cast<T>(1.0);
                        framebuffer.moveCursorLeft();
                    }
                    framebuffer.moveCursorDown();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
        }

        // if (x1 > x0) {
        //     const T dx = x1 - x0;
        //     if (dx >= dy) {
        //         // Line moves in x faster than in y.
        //         const auto x_start = static_cast<uint32>(std::round(x0));
        //         const auto x_end = static_cast<uint32>(std::round(x1));
        //         const T y_step = dy / dx;
        //         T y = y0;
        //         T y_raster = std::round(y);
        //         framebuffer.setCursor(x_start, static_cast<uint32>(y_raster));
        //         for (uint32 x = x_start + 1; x <= x_end; ++x) {
        //             y += y_step;
        //             if (y - y_raster >= static_cast<T>(0.5)) {
        //                 y_raster += static_cast<T>(1.0);
        //                 framebuffer.moveCursorDown();
        //             }
        //             framebuffer.moveCursorRight();
        //             framebuffer.drawPixelAtCursor(color);
        //         }
        //     }
        //     else {
        //         // Line moves in x slower than in y.
        //         const auto y_start = static_cast<uint32>(std::round(y0));
        //         const auto y_end = static_cast<uint32>(std::round(y1));
        //         const T x_step = dx / dy;
        //         T x = x0;
        //         T x_raster = std::round(x);
        //         framebuffer.setCursor(static_cast<uint32>(x_raster), y_start);
        //         for (uint32 y = y_start + 1; y <= y_end; ++y) {
        //             x += x_step;
        //             if (x - x_raster >= static_cast<T>(0.5)) {
        //                 x_raster += static_cast<T>(1.0);
        //                 framebuffer.moveCursorRight();
        //             }
        //             framebuffer.moveCursorDown();
        //             framebuffer.drawPixelAtCursor(color);
        //         }
        //     }
        // }
        // else {
        //     const T dx = x0 - x1;
        //     if (dx >= dy) {
        //         // Line moves in x faster than in y.
        //         const auto x_start = static_cast<uint32>(std::round(x0));
        //         const auto x_end = static_cast<uint32>(std::round(x1));
        //         const T y_step = dy / dx;
        //         T y = y0;
        //         T y_raster = std::round(y);
        //         framebuffer.setCursor(x_start, static_cast<uint32>(y_raster));
        //         for (uint32 x = x_start - 1; x >= x_end; --x) {
        //             y += y_step;
        //             if (y - y_raster >= static_cast<T>(0.5)) {
        //                 y_raster += static_cast<T>(1.0);
        //                 framebuffer.moveCursorDown();
        //             }
        //             framebuffer.moveCursorLeft();
        //             framebuffer.drawPixelAtCursor(color);
        //         }
        //     }
        //     else {
        //         // Line moves in x slower than in y.
        //         const auto y_start = static_cast<uint32>(std::round(y0));
        //         const auto y_end = static_cast<uint32>(std::round(y1));
        //         const T x_step = dx / dy;
        //         T x = x0;
        //         T x_raster = std::round(x);
        //         framebuffer.setCursor(static_cast<uint32>(x_raster), y_start);
        //         for (uint32 y = y_start + 1; y <= y_end; ++y) {
        //             x -= x_step;
        //             if (x_raster - x >= static_cast<T>(0.5)) {
        //                 x_raster -= static_cast<T>(1.0);
        //                 framebuffer.moveCursorLeft();
        //             }
        //             framebuffer.moveCursorDown();
        //             framebuffer.drawPixelAtCursor(color);
        //         }
        //     }
        // }
    }
} // namespace MicroRenderer
