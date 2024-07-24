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
            T y_thresh = std::round(y);
            framebuffer.setCursor(x_start, static_cast<uint32>(y_thresh));
            y_thresh += static_cast<T>(0.5);
            if (dx_abs == dx) {
                // Line moves to the right.
                for (uint32 x = x_start + 1; x <= x_end; ++x) {
                    y += y_step;
                    if (y >= y_thresh) {
                        y_thresh += static_cast<T>(1.0);
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
                    if (y >= y_thresh) {
                        y_thresh += static_cast<T>(1.0);
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
            T x_thresh = std::round(x);
            framebuffer.setCursor(static_cast<uint32>(x_thresh), y_start);
            if (dx == dx_abs) {
                // Line moves to the right.
                x_thresh += static_cast<T>(0.5);
                for (uint32 y = y_start + 1; y <= y_end; ++y) {
                    x += x_step;
                    if (x >= x_thresh) {
                        x_thresh += static_cast<T>(1.0);
                        framebuffer.moveCursorRight();
                    }
                    framebuffer.moveCursorDown();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
            else {
                // Line moves to the left.
                x_thresh -= static_cast<T>(0.5);
                for (uint32 y = y_start + 1; y <= y_end; ++y) {
                    x -= x_step;
                    if (x_thresh >= x) {
                        x_thresh -= static_cast<T>(1.0);
                        framebuffer.moveCursorLeft();
                    }
                    framebuffer.moveCursorDown();
                    framebuffer.drawPixelAtCursor(color);
                }
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    template<template<typename> class ShaderClass>
    void Renderer<T, color_coding>::rasterizeTriangle(const Vector3<T>* v1, const Vector3<T>* v2, const Vector3<T>* v3) {
        // Triangle is split into upper and lower half-triangle, which are rasterized/shaded separately.
        // Upper half: scanlines contained between top-most two edges.
        // Lower half: scanlines contained between bottom-most two edges.

        // Initialize shader.
        ShaderClass<T> shader;
        if (!shader.initializeShading(v1, v2, v3)) {
            return;
        }

        // Define lamba that implements the "top-left" rule for rasterizing edges going exactly through pixel points.
        auto isTopOrLeftEdge = [](const T delta_x, const T delta_y) -> bool {
            const bool isLeftEdge = delta_y > 0;
            const bool isTopEdge = delta_y == 0 && delta_x < 0;
            return isLeftEdge || isTopEdge;
        };

        // Define lambda that draws lines between two edges.
        auto rasterizeHalfTriangle = [&, this](uint32 y_start, uint32 y_stop, T& left_edge_x, T& right_edge_x, T left_x_step, T right_x_step) {
            for (uint32 y = y_start; y < y_stop; ++y) {
                // Set start and end of next line.
                uint32 x_start = std::ceil(left_edge_x);
                uint32 x_stop = std::floor(right_edge_x);

                // Advance framebuffer and shader to start of next line.
                framebuffer.setCursor(x_start, y);
                shader.setPosition(x_start, y);

                // Shade and draw rasterized line.
                framebuffer.drawPixelAtCursor(shader.computeShading());
                for (uint32 j = x_start; j < x_stop; ++j) {
                    framebuffer.moveCursorRight();
                    shader.movePositionRight();
                    framebuffer.drawPixelAtCursor(shader.computeShading());
                }

                // Compute x-coords of edges at next scanline.
                left_edge_x += left_x_step;
                right_edge_x += right_x_step;
            }
        };

        // Sort vertices by y: v1->y < v2->y < v3->y.
        if (v1->y > v3->y)
            std::swap(v1, v3);
        if (v1->y > v2->y)
            std::swap(v1, v2);
        if (v2->y > v3->y)
            std::swap(v2, v3);

        // Compute edge deltas.
        const T dx_top_middle = v2->x - v1->x;
        const T dy_top_middle = v2->y - v1->y;
        const T dx_top_bottom = v3->x - v1->x;
        const T dy_top_bottom = v3->y - v1->y;
        const T dy_middle_bottom = v3->y - v2->y;
        const T dx_middle_bottom = v3->x - v2->x;

        // Check for degenerate triangle.
        constexpr T DEGENERATE_THRESHOLD = static_cast<T>(0.001);
        if (std::abs(dx_top_middle * dy_top_bottom - dy_top_middle * dx_top_bottom) < DEGENERATE_THRESHOLD)
            return;

        // Compute start and stop y-coords of upper and lower half-triangle.
        const T y_start = std::ceil(v1->y);
        const T y_middle = std::ceil(v2->y);
        const T y_end = std::floor(v3->y) + static_cast<T>(1.0);

        // // Adjust edge x-coords according to "top-left" rule.
        // T x_start_left_edge = v1->x;
        // if (dy_top_middle == static_cast<T>(0.0)) {
        //     // v1-v2 is top edge and v1-v3 is left edge.
        //     v1->x += std::numeric_limits<T>::min();
        //     v3->x += std::numeric_limits<T>::min();
        // }
        // else {
        //     if (dx_middle_bottom > static_cast<T>(0.0)) {
        //         // v1-v2 is left edge.
        //         v1->x += std::numeric_limits<T>::min();
        //         v2->x += std::numeric_limits<T>::min();
        //         if (dy_middle_bottom > static_cast<T>(0.0)) {
        //             // v2-v3 is left edge.
        //             v1->x += std::numeric_limits<T>::min();
        //         }
        //     }
        //     else {
        //         // v1-v3 is left edge.
        //     }
        // }

        // Compute edge properties and rasterize/shade.
        T left_edge_x;
        T right_edge_x;
        T right_x_step;
        T left_x_step;
        const bool swap_edges = dx_top_middle > dx_top_bottom;
        constexpr T MIN_DELTA_Y = static_cast<T>(0.001);
        if (y_middle > y_start && dy_top_middle > MIN_DELTA_Y) { // Does upper half-triangle exist?
            // Compute x-coords and inverse slopes of edges at first scanline of upper half triangle.
            if (swap_edges) {
                left_x_step = dx_top_bottom / dy_top_bottom;
                right_x_step = dx_top_middle / dy_top_middle;
            }
            else {
                left_x_step = dx_top_middle / dy_top_middle;
                right_x_step = dx_top_bottom / dy_top_bottom;
            }
            left_edge_x = v1->x + left_x_step * (y_start - v1->y);
            right_edge_x = v1->x + right_x_step * (y_start - v1->y);

            // Draw upper half-triangle.
            rasterizeHalfTriangle(static_cast<uint32>(y_start), static_cast<uint32>(y_middle), left_edge_x, right_edge_x, left_x_step, right_x_step);

            if (y_end > y_middle && dy_middle_bottom > MIN_DELTA_Y) { // Does lower half-triangle exist?
                // Compute x-coord and inverse slope of new edge at first scanline of lower half triangle.
                if (swap_edges) {
                    right_x_step = dx_middle_bottom / dy_middle_bottom;
                    right_edge_x = v2->x + right_x_step * (y_middle - v2->y);
                }
                else {
                    left_x_step = dx_middle_bottom / dy_middle_bottom;
                    left_edge_x = v2->x + left_x_step * (y_middle - v2->y);
                }

                // Draw lower half-triangle.
                rasterizeHalfTriangle(static_cast<uint32>(y_middle), static_cast<uint32>(y_end), left_edge_x, right_edge_x, left_x_step, right_x_step);
            }
        }
        else if (y_end > y_middle && dy_middle_bottom > MIN_DELTA_Y) { // Does lower half-triangle exist?
            // Compute x-coords and inverse slopes of edges at first scanline of lower half triangle.
            if (swap_edges) {
                left_x_step = dx_top_bottom / dy_top_bottom;
                right_x_step = dx_middle_bottom / dy_middle_bottom;
                left_edge_x = v1->x + left_x_step * (y_middle - v1->y);
                right_edge_x = v2->x + right_x_step * (y_middle - v2->y);
            }
            else {
                left_x_step = dx_middle_bottom / dy_middle_bottom;
                right_x_step = dx_top_bottom / dy_top_bottom;
                left_edge_x = v2->x + right_x_step * (y_middle - v2->y);
                right_edge_x = v1->x + right_x_step * (y_middle - v1->y);
            }

            // Draw lower half-triangle.
            rasterizeHalfTriangle(static_cast<uint32>(y_middle), static_cast<uint32>(y_end), left_edge_x, right_edge_x, left_x_step, right_x_step);
        }
    }
} // namespace MicroRenderer
