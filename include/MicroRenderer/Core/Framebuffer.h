//
// Created by pusdo on 16/07/2024.
//

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

#define RGB444_ALIGNMENT_EVEN 0
#define RGB444_ALIGNMENT_ODD 1

enum ColorCoding {
    RGB888,
    RGB565,
    RGB444
};

template<typename T, ColorCoding color_coding>
class Framebuffer {
public:

    Framebuffer(uint32 width, uint32 height, uint8* address);

    void setResolution(uint32 width, uint32 height);

    uint32 getWidth() const {
        return buffer_width;
    }

    uint32 getHeight() const {
        return buffer_height;
    }

    void setAddress(uint8* address);

    uint8* getAddress() const {
        return buffer;
    }

    void drawPixel(uint32 x, const Vector3<T> &color);

    void drawPixel(uint32 x, uint32 y, const Vector3<T> &color);

    void drawNextPixelInX(const Vector3<T> &color);

    void drawNextPixelInY(const Vector3<T> &color);

    void drawNextPixelInXY(const Vector3<T> &color);
private:

    void drawPixelAtNumber(uint32 pixel_number, const Vector3<T> &color);

    void drawRGB888AtCursor(const Vector3<T> &color);

    void drawRGB565AtCursor(const Vector3<T> &color);

    void drawRGB444AtCursorEven(const Vector3<T> &color);

    void drawRGB444AtCursorOdd(const Vector3<T> &color);

    // Buffer width.
    uint32 buffer_width = 0;

    // Buffer height.
    uint32 buffer_height = 0;

    // Pointer to buffer memory.
    uint8* buffer = nullptr;

    // Pointer to a specific pixel in buffer.
    uint8* pixel_cursor = nullptr;

    // Helper variable to allow efficient x-incremental drawing to buffer in RGB444 mode.
    uint8 rgb444_last_alignment = RGB444_ALIGNMENT_EVEN;

    // Helper variable to allow efficient y-incremental drawing to buffer.
    uint32 next_line_cursor_increment = 0;
};

} // namespace MicroRenderer
