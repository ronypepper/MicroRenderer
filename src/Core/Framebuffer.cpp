//
// Created by pusdo on 16/07/2024.
//

#include "MicroRenderer/Core/Framebuffer.h"

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
        if (color_coding == RGB888) {
            next_line_cursor_increment = buffer_width * 3;
        }
        else if (color_coding == RGB565) {
            next_line_cursor_increment = buffer_width * 2;
        }
        else if (color_coding == RGB444) {
            // Make sure buffer_width is even for RGB444 mode.
            buffer_width &= 0xFFFFFFFE;
            next_line_cursor_increment = (buffer_width / 2) * 3;
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setAddress(uint8 *address) {
        buffer = address;
        pixel_cursor = address;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawPixel(uint32 x, const Vector3<T> &color) {
        drawPixelAtNumber(x);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawPixel(uint32 x, uint32 y, const Vector3<T> &color) {
        drawPixelAtNumber(x + buffer_width * y);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawNextPixelInX(const Vector3<T> &color) {
        if (color_coding == RGB888) {
            pixel_cursor += 3;
            drawRGB888AtCursor(color);
        }
        else if (color_coding == RGB565) {
            pixel_cursor += 2;
            drawRGB565AtCursor(color);
        }
        else if (color_coding == RGB444) {
            if (rgb444_last_alignment) { // == RGB444_ALIGNMENT_ODD
                pixel_cursor += 2;
                drawRGB444AtCursorEven(color);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel_cursor += 1;
                drawRGB444AtCursorOdd(color);
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawNextPixelInY(const Vector3<T> &color) {
        pixel_cursor += next_line_cursor_increment;
        if (color_coding == RGB888) {
            drawRGB888AtCursor(color);
        }
        else if (color_coding == RGB565) {
            drawRGB565AtCursor(color);
        }
        else if (color_coding == RGB444) {
            if (rgb444_last_alignment) { // == RGB444_ALIGNMENT_ODD
                drawRGB444AtCursorOdd(color);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                drawRGB444AtCursorEven(color);
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawNextPixelInXY(const Vector3<T> &color) {
        pixel_cursor += next_line_cursor_increment;
        if (color_coding == RGB888) {
            pixel_cursor += 3;
            drawRGB888AtCursor(color);
        }
        else if (color_coding == RGB565) {
            pixel_cursor += 2;
            drawRGB565AtCursor(color);
        }
        else if (color_coding == RGB444) {
            if (rgb444_last_alignment) { // == RGB444_ALIGNMENT_ODD
                pixel_cursor += 2;
                drawRGB444AtCursorEven(color);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel_cursor += 1;
                drawRGB444AtCursorOdd(color);
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawPixelAtNumber(uint32 pixel_number, const Vector3<T> &color) {
        if (color_coding == RGB888) {
            pixel_cursor = buffer + pixel_number * 3;
            drawRGB888AtCursor(color);
        }
        else if (color_coding == RGB565) {
            pixel_cursor = buffer + pixel_number * 2;
            drawRGB565AtCursor(color);
        }
        else if (color_coding == RGB444) {
            if (pixel_number << 31) {
                pixel_cursor = buffer + (pixel_number * 3 + 1);
                drawRGB444AtCursorOdd(color);
            }
            else {
                pixel_cursor = buffer + pixel_number * 3;
                drawRGB444AtCursorEven(color);
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB888AtCursor(const Vector3<T> &color) {
        pixel_cursor[0] = static_cast<uint8>(color.r * 255);
        pixel_cursor[1] = static_cast<uint8>(color.g * 255);
        pixel_cursor[2] = static_cast<uint8>(color.b * 255);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB565AtCursor(const Vector3<T> &color) {
        uint16 coded_color = static_cast<uint16>(color.r * 31) << 11;
        coded_color |= static_cast<uint16>(color.g * 63) << 5;
        coded_color |= static_cast<uint16>(color.b * 31);
        *reinterpret_cast<uint16*>(pixel_cursor) = coded_color;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB444AtCursorEven(const Vector3<T> &color) {
        // Pixel RGB spans: RGBr
        rgb444_last_alignment = RGB444_ALIGNMENT_EVEN;
        uint8 coded_color = static_cast<uint8>(color.r * 15) << 4;
        coded_color |= static_cast<uint8>(color.g * 15);
        pixel_cursor[0] = coded_color;
        pixel_cursor[1] &= 0x0F;
        pixel_cursor[1] |= static_cast<uint8>(color.b * 15) << 4;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB444AtCursorOdd(const Vector3<T> &color) {
        // Pixel RGB spans: bRGB
        rgb444_last_alignment = RGB444_ALIGNMENT_ODD;
        pixel_cursor[0] &= 0xF0;
        pixel_cursor[0] |= static_cast<uint8>(color.r * 15);
        uint8 coded_color = static_cast<uint8>(color.g * 15) << 4;
        coded_color |= static_cast<uint8>(color.b * 15);
        pixel_cursor[1] = coded_color;
    }
} // namespace MicroRenderer
