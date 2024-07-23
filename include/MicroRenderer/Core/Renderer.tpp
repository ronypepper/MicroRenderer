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
        // Define lambda that draws lines between two edges.
        T left_edge_x;
        T right_edge_x;
        ShaderClass<T> shader;
        auto rasterizeHalfTriangle = [&, this](uint32 num_of_lines, uint32 left_x_step, uint32 right_x_step) -> T {
            // Set start and end of first line.
            uint32 x_start = std::ceil(left_edge_x);
            uint32 x_stop = std::floor(right_edge_x);
            for (uint32 i = 0; i < num_of_lines; ++i) {
                // Check if start/stop is exactly on edge.
                //TODO

                // Shade and draw rasterized line.
                const uint32 line_length = x_stop - x_start;
                for (uint32 j = 0; j <= line_length; ++j) {
                    framebuffer.drawPixelAtCursor(shader.computeShading());
                    framebuffer.moveCursorRight();
                    shader.advanceShadingInX();
                }

                // Compute x-coords of edges at next scanline.
                left_edge_x += left_x_step;
                right_edge_x += right_x_step;

                // Set start of next line.
                x_start = std::ceil(left_edge_x);

                // Advance framebuffer and shader to start of next line.
                const uint32 delta_x = x_stop - x_start; //TODO: +1?
                framebuffer.moveCursorLeftBy(delta_x);
                shader.advanceShadingInXBy(delta_x);

                // Set end of next line.
                x_stop = std::floor(right_edge_x);

                // Advance framebuffer and shader to next scanline.
                framebuffer.moveCursorDown();
                shader.advanceShadingInY();
            }
            return x_stop;
        };

        // Sort vertices by y: v1->y < v2->y < v3->y.
        if (v1->y > v3->y)
            std::swap(v1, v3);
        if (v1->y > v2->y)
            std::swap(v1, v2);
        if (v2->y > v3->y)
            std::swap(v2, v3);

        // Compute initial y-coord.
        // Compute last y-coord. rasterized between top-most two edges.
        const T y_start = std::ceil(v1->y);
        const T y_middle = std::floor(v2->y);
        const T y_end = std::floor(v3->y);
        const uint32 top_tri_line_num = y_middle - y_start;
        const uint32 bottom_tri_line_num = y_end - y_middle;

        // Compute inverse slopes of topmost edges.
        const T delta_y_top = v2->y - v1->y;
        const T delta_y_middle = v3->y - v1->y;
        const T delta_y_bottom = v3->y - v2->y;

        if (top_tri_line_num > 0) {
            // delta_y_middle is also != 0.
            T left_x_step = (v2->x - v1->x) / delta_y_top;
            T right_x_step = (v3->x - v1->x) / delta_y_middle;
            bool left_edge_extends_lower = false;
            if (left_x_step > right_x_step) {
                left_edge_extends_lower = true;
                std::swap(left_x_step, right_x_step);
            }

            // Compute x-coords of top-most two edges at initial y-coord.
            left_edge_x = v1->x + left_x_step * (y_start - v1->y);
            right_edge_x = v1->x + right_x_step * (y_start - v1->y);

            // Initialize framebuffer position and shader.
            const uint32 x_start = std::ceil(left_edge_x);
            framebuffer.setCursor(x_start, y_start);
            shader.initializeShading(v1, v2, v3, x_start, y_start);

            // Draw upper half of triangle (horizontally between top-most two edges).
            rasterizeHalfTriangle(top_tri_line_num + 1);

            if (bottom_tri_line_num > 0) {
                // Change inv.slope, x-coord and last y-coord to rasterize lower half of triangle.
                const T new_edge_x_step = (v3->x - v2->x) / delta_y_bottom;
                const T new_edge_x = v2->x + new_edge_x_step * (y_middle + 1 - v2->y);
                if (left_edge_extends_lower) {
                    right_x_step = new_edge_x_step;
                    right_edge_x = new_edge_x;
                }
                else {
                    left_x_step = new_edge_x_step;
                    left_edge_x = new_edge_x;
                }

                // Draw lower half of triangle (horizontally between bottom-most two edges).
                rasterizeHalfTriangle(bottom_tri_line_num);
            }
        }
        else if (bottom_tri_line_num > 0) {

        }

        if (delta_y_top != 0.0) { //TODO: epsilon

        }
        else {

        }
    }
} // namespace MicroRenderer
