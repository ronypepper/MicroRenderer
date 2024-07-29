//
// Created by pusdo on 16/07/2024.
//

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector3.h"
#include <string>

namespace MicroRenderer {

#define RGB444_ALIGNMENT_EVEN 0
#define RGB444_ALIGNMENT_ODD 1

enum ColorCoding {
    RGB888 = 1u << 0,
    RGB565 = 1u << 1,
    RGB444 = 1u << 2,
    BGR888 = 1u << 3,
    BGR565 = 1u << 4,
    BGR444 = 1u << 5
};

template<typename T, ColorCoding COLOR_CODING>
class Framebuffer {
public:

    Framebuffer(int32 width, int32 height, uint8* address);

    void setResolution(int32 width, int32 height);

    int32 getWidth() const {
        return buffer_width;
    }

    int32 getHeight() const {
        return buffer_height;
    }

    void setAddress(uint8* address);

    uint8* getAddress() const {
        return buffer;
    }

    void clearBuffer();

    void fillRect(int32 x0, int32 y0, int32 x1, int32 y1, const Vector3<T> &color);

    void setCursor(int32 x);

    void setCursor(int32 x, int32 y);

    void moveCursorRight();

    void moveCursorRightBy(int32 delta_x);

    void moveCursorLeft();

    void moveCursorLeftBy(int32 delta_x);

    void moveCursorUp();

    void moveCursorUpBy(int32 delta_y);

    void moveCursorDown();

    void moveCursorDownBy(int32 delta_y);

    void drawPixelAtCursor(const Vector3<T>& color);

    Vector3<uint8> getPixelAtCursor();

    void saveToPPMImage(const std::string& file_name);

private:
    void drawRGB888AtCursor(const Vector3<T>& color);

    void drawRGB565AtCursor(const Vector3<T>& color);

    void drawRGB444AtCursor(const Vector3<T>& color);

    bool verifyCursorInsideBuffer(const uint8* cursor) const;

    // Buffer width.
    int32 buffer_width = 0;

    // Buffer height.
    int32 buffer_height = 0;

    // Pointer to buffer memory.
    uint8* buffer = nullptr;

    // Pointer to a specific pixel in buffer.
    uint8* pixel_cursor = nullptr;

    // Helper variable to allow efficient x-incremental drawing to buffer in RGB444 mode.
    uint32 rgb444_alignment = RGB444_ALIGNMENT_EVEN;

    // Helper variable to allow efficient y-incremental drawing to buffer.
    int32 next_line_cursor_increment = 0;
};

} // namespace MicroRenderer

#include "Framebuffer.tpp"
