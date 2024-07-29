//
// Created by pusdo on 16/07/2024.
//

#pragma once
#include <cassert>
#include <cstring>
#include <fstream>

namespace MicroRenderer {
    template<typename T, ColorCoding color_coding>
    Framebuffer<T, color_coding>::Framebuffer(int32 width, int32 height, uint8* address) {
        setResolution(width, height);
        setAddress(address);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setResolution(int32 width, int32 height) {
        assert(width > 0 && height > 0);
        buffer_width = width;
        buffer_height = height;
        if constexpr (color_coding & (RGB888 | BGR888)) {
            next_line_cursor_increment = buffer_width * 3;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            next_line_cursor_increment = buffer_width * 2;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            // Make sure buffer_width is even for RGB444/BGR444 mode.
            assert(height & 0xFFFFFFFE);
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
    void Framebuffer<T, color_coding>::fillRect(int32 x0, int32 y0, int32 x1, int32 y1, const Vector3<T> &color) {

    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setCursor(int32 x) {
        assert(x >= 0);
        if constexpr (color_coding & (RGB888 | BGR888)) {
            pixel_cursor = buffer + x * 3;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            pixel_cursor = buffer + x * 2;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            pixel_cursor = buffer + (x >> 1) * 3;
            if (x << 31) {
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
            else {
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::setCursor(int32 x, int32 y) {
        assert(x >= 0 && y >= 0);
        setCursor(x + buffer_width * y);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorRight() {
        if constexpr (color_coding & (RGB888 | BGR888)) {
            pixel_cursor += 3;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            pixel_cursor += 2;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                pixel_cursor += 3;
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorRightBy(int32 delta_x) {
        assert(delta_x > 0);
        if constexpr (color_coding & (RGB888 | BGR888)) {
            pixel_cursor += 3 * delta_x;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            pixel_cursor += 2 * delta_x;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            pixel_cursor += (delta_x >> 1) * 3;
            if (delta_x << 31)
                moveCursorRight();
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorLeft() {
        if constexpr (color_coding & (RGB888 | BGR888)) {
            pixel_cursor -= 3;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            pixel_cursor -= 2;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                rgb444_alignment = RGB444_ALIGNMENT_EVEN;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel_cursor -= 3;
                rgb444_alignment = RGB444_ALIGNMENT_ODD;
            }
        }
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorLeftBy(int32 delta_x) {
        assert(delta_x > 0);
        if constexpr (color_coding & (RGB888 | BGR888)) {
            pixel_cursor -= 3 * delta_x;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            pixel_cursor -= 2 * delta_x;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
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
    void Framebuffer<T, color_coding>::moveCursorUpBy(int32 delta_y) {
        assert(delta_y > 0);
        pixel_cursor -= next_line_cursor_increment * delta_y;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorDown() {
        pixel_cursor += next_line_cursor_increment;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::moveCursorDownBy(int32 delta_y) {
        assert(delta_y > 0);
        pixel_cursor += next_line_cursor_increment * delta_y;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawPixelAtCursor(const Vector3<T> &color) {
        if constexpr (color_coding & (RGB888 | BGR888)) {
            drawRGB888AtCursor(color);
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            drawRGB565AtCursor(color);
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            drawRGB444AtCursor(color);
        }
    }

    template<typename T, ColorCoding color_coding>
    Vector3<uint8> Framebuffer<T, color_coding>::getPixelAtCursor() {
        Vector3<uint8> color;
        if constexpr (color_coding & (RGB888 | BGR888)) {
            assert(verifyCursorInsideBuffer(pixel_cursor));
            assert(verifyCursorInsideBuffer(pixel_cursor + 1));
            assert(verifyCursorInsideBuffer(pixel_cursor + 2));
            color.r = pixel_cursor[0];
            color.g = pixel_cursor[1];
            color.b = pixel_cursor[2];
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            assert(verifyCursorInsideBuffer(pixel_cursor));
            assert(verifyCursorInsideBuffer(pixel_cursor + 1));
            const uint16* cast_cursor = reinterpret_cast<uint16*>(pixel_cursor);
            color.r = static_cast<uint8>((*cast_cursor & 0xF800) >> 8);
            color.g = static_cast<uint8>((*cast_cursor & 0x07E0) >> 3);
            color.b = static_cast<uint8>((*cast_cursor & 0x001F) << 3);
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            assert(verifyCursorInsideBuffer(pixel_cursor));
            assert(verifyCursorInsideBuffer(pixel_cursor + 1));
            assert(verifyCursorInsideBuffer(pixel_cursor + 2));
            if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
                color.r = (pixel_cursor[1] & 0x0F) << 4;
                color.g = pixel_cursor[2] & 0xF0;
                color.b = (pixel_cursor[2] & 0x0F) << 4;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                color.r = pixel_cursor[0] & 0xF0;
                color.g = (pixel_cursor[0] & 0x0F) << 4;
                color.b = pixel_cursor[1] & 0xF0;
            }
        }
        if constexpr (color_coding & (BGR888 | BGR565 | BGR444)) {
            uint8 temp = color.r;
            color.r = color.b;
            color.b = temp;
        }
        return color;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::saveToPPMImage(const std::string& file_name)
    {
        // Check parameters and open file.
        if (!buffer || buffer_width < 1 || buffer_height < 1 || buffer_width > 5000 || buffer_height > 5000) {
            assert(false && "Invalid parameters when trying to save framebuffer to ppm file!");
            return;
        }
        std::ofstream file(file_name + ".ppm" , std::ios_base::out | std::ios_base::binary);
        file << "P3\n" << buffer_width << ' ' << buffer_height << "\n255\n";;
        if (!file.is_open()) {
            assert(false && "Image file could not be opened!");
            return;
        }

        // Write framebuffer to file.
        setCursor(0, 0);
        for (uint32 i = 0; i < buffer_height * buffer_width; ++i) {
            const Vector3<uint8> color = getPixelAtCursor();
            if constexpr (color_coding & (RGB888 | RGB565 | RGB444)) {
                file << std::to_string(color.r) << " " << std::to_string(color.g) << " " << std::to_string(color.b) << "\n";
            }
            else {
                file << std::to_string(color.b) << " " << std::to_string(color.g) << " " << std::to_string(color.r) << "\n";
            }
            moveCursorRight();
        }
        setCursor(0, 0);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB888AtCursor(const Vector3<T> &color) {
        assert(verifyCursorInsideBuffer(pixel_cursor));
        assert(verifyCursorInsideBuffer(pixel_cursor + 1));
        assert(verifyCursorInsideBuffer(pixel_cursor + 2));
        const T red = color_coding == RGB888 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB888 ? color.b : color.r;
        pixel_cursor[0] = static_cast<uint8>(red * 255);
        pixel_cursor[1] = static_cast<uint8>(green * 255);
        pixel_cursor[2] = static_cast<uint8>(blue * 255);
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB565AtCursor(const Vector3<T> &color) {
        assert(verifyCursorInsideBuffer(pixel_cursor));
        assert(verifyCursorInsideBuffer(pixel_cursor + 1));
        const T red = color_coding == RGB565 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB565 ? color.b : color.r;
        uint16 coded_color = static_cast<uint16>(red * 31) << 11;
        coded_color |= static_cast<uint16>(green * 63) << 5;
        coded_color |= static_cast<uint16>(blue * 31);
        *reinterpret_cast<uint16*>(pixel_cursor) = coded_color;
    }

    template<typename T, ColorCoding color_coding>
    void Framebuffer<T, color_coding>::drawRGB444AtCursor(const Vector3<T> &color) {
        assert(verifyCursorInsideBuffer(pixel_cursor));
        assert(verifyCursorInsideBuffer(pixel_cursor + 1));
        assert(verifyCursorInsideBuffer(pixel_cursor + 2));
        const T red = color_coding == RGB444 ? color.r : color.b;
        const T green = color.g;
        const T blue = color_coding == RGB444 ? color.b : color.r;
        if (rgb444_alignment) { // == RGB444_ALIGNMENT_ODD
            // Pixel RGB spans: bRGB
            pixel_cursor[1] &= 0xF0;
            pixel_cursor[1] |= static_cast<uint8>(red * 15);
            pixel_cursor[2] = static_cast<uint8>(green * 15) << 4;
            pixel_cursor[2] |= static_cast<uint8>(blue * 15);
        }
        else { // == RGB444_ALIGNMENT_EVEN
            // Pixel RGB spans: RGBr
            pixel_cursor[0] = static_cast<uint8>(red * 15) << 4;
            pixel_cursor[0] |= static_cast<uint8>(green * 15);
            pixel_cursor[1] &= 0x0F;
            pixel_cursor[1] |= static_cast<uint8>(blue * 15) << 4;
        }
    }

    template<typename T, ColorCoding color_coding>
    bool Framebuffer<T, color_coding>::verifyCursorInsideBuffer(const uint8* cursor) const
    {
        if (buffer == nullptr || cursor < buffer)
            return false;
        if constexpr (color_coding & (RGB888 | BGR888)) {
            if (cursor >= buffer + buffer_width * buffer_height * 3)
                return false;
        }
        else if constexpr (color_coding & (RGB565 | BGR565)) {
            if (cursor >= buffer + buffer_width * buffer_height * 2)
                return false;
        }
        else if constexpr (color_coding & (RGB444 | BGR444)) {
            if (cursor >= buffer + (buffer_width * buffer_height << 1) * 3)
                return false;
        }
        return true;
    }
} // namespace MicroRenderer
