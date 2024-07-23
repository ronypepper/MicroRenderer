//
// Created by pusdo on 16/07/2024.
//

#pragma once
//#include "Framebuffer.h"
#include <cstring>

namespace MicroRenderer {
    template<typename T, ColorCoding color_coding>
    Framebuffer<T, color_coding>::Framebuffer(uint32 width, uint32 height, uint8* address) {
        setResolution(width, height);
        setAddress(address);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setResolution(uint32 width, uint32 height) {
        buffer_width = width;
        buffer_height = height;
        if (color_coding & (RGB888 | BGR888)) {
            next_line_cursor_increment = buffer_width * 3;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            next_line_cursor_increment = buffer_width * 2;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            // Make sure buffer_width is even for RGB444/BGR444 mode.
            buffer_width &= 0xFFFFFFFE;
            next_line_cursor_increment = (buffer_width >> 1) * 3;
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setAddress(uint8 *address) {
        buffer = address;
        pixel_cursor = address;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::clearBuffer() {
        std::memset(buffer, 0, buffer_height * next_line_cursor_increment);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::fillRect(uint32 x0, uint32 y0, uint32 x1, uint32 y1, const Vector3<T> &color) {

    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setCursor(uint32 x) {
        if (color_coding & (RGB888 | BGR888)) {
            pixel_cursor = buffer + x * 3;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            pixel_cursor = buffer + x * 2;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            pixel_cursor = buffer + (x >> 1) * 3;
            if (x << 31) {
                pixel_cursor = buffer + 1;
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
            else {
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setCursor(uint32 x, uint32 y) {
        setCursor(x + buffer_width * y);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorRight() {
        if (color_coding & (RGB888 | BGR888)) {
            pixel_cursor += 3;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            pixel_cursor += 2;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                pixel_cursor += 2;
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel_cursor += 1;
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorRightBy(uint32 delta_x) {
        if (color_coding & (RGB888 | BGR888)) {
            pixel_cursor += 3 * delta_x;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            pixel_cursor += 2 * delta_x;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            pixel_cursor += (delta_x >> 1) * 3;
            if (delta_x << 31)
                moveCursorRight();
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorLeft() {
        if (color_coding & (RGB888 | BGR888)) {
            pixel_cursor -= 3;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            pixel_cursor -= 2;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                pixel_cursor -= 1;
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel_cursor -= 2;
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorLeftBy(uint32 delta_x) {
        if (color_coding & (RGB888 | BGR888)) {
            pixel_cursor -= 3 * delta_x;
        }
        else if (color_coding & (RGB565 | BGR565)) {
            pixel_cursor -= 2 * delta_x;
        }
        else if (color_coding & (RGB444 | BGR444)) {
            pixel_cursor -= (delta_x >> 1) * 3;
            if (delta_x << 31)
                moveCursorLeft();
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorUp() {
        pixel_cursor -= next_line_cursor_increment;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorUpBy(uint32 delta_y) {
        pixel_cursor -= next_line_cursor_increment * delta_y;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorDown() {
        pixel_cursor += next_line_cursor_increment;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorDownBy(uint32 delta_y) {
        pixel_cursor += next_line_cursor_increment * delta_y;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawPixelAtCursor(const Vector3<T> &color) {
        if (color_coding & (RGB888 | BGR888)) {
            drawRGB888AtCursor(color);
        }
        else if (color_coding & (RGB565 | BGR565)) {
            drawRGB565AtCursor(color);
        }
        else if (color_coding & (RGB444 | BGR444)) {
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                drawRGB444AtCursorOdd(color);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                drawRGB444AtCursorEven(color);
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB888AtCursor(const Vector3<T> &color) {
        const T red = color_coding == RGB888 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB888 ? color.b : color.r;
        pixel_cursor[0] = static_cast<uint8>(red * 255);
        pixel_cursor[1] = static_cast<uint8>(green * 255);
        pixel_cursor[2] = static_cast<uint8>(blue * 255);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB565AtCursor(const Vector3<T> &color) {
        const T red = color_coding == RGB565 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB565 ? color.b : color.r;
        uint16 coded_color = static_cast<uint16>(red * 31) << 11;
        coded_color |= static_cast<uint16>(green * 63) << 5;
        coded_color |= static_cast<uint16>(blue * 31);
        *reinterpret_cast<uint16*>(pixel_cursor) = coded_color;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB444AtCursorEven(const Vector3<T> &color) {
        // Pixel RGB spans: RGBr
        const T red = color_coding == RGB444 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB444 ? color.b : color.r;
        uint8 coded_color = static_cast<uint8>(red * 15) << 4;
        coded_color |= static_cast<uint8>(green * 15);
        pixel_cursor[0] = coded_color;
        pixel_cursor[1] &= 0x0F;
        pixel_cursor[1] |= static_cast<uint8>(blue * 15) << 4;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB444AtCursorOdd(const Vector3<T> &color) {
        // Pixel RGB spans: bRGB
        const T red = color_coding == RGB444 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB444 ? color.b : color.r;
        pixel_cursor[0] &= 0xF0;
        pixel_cursor[0] |= static_cast<uint8>(red * 15);
        uint8 coded_color = static_cast<uint8>(green * 15) << 4;
        coded_color |= static_cast<uint8>(blue * 15);
        pixel_cursor[1] = coded_color;
    }
} // namespace MicroRenderer
