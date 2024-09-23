//
// Created by pusdo on 15/08/2024.
//

#pragma once
#include "Texture2D.h"
#include <algorithm>
#include <cassert>
#include <fstream>

namespace MicroRenderer {

    template <typename T, TextureConfiguration t_cfg>
    Texture2D<T, t_cfg>::Texture2D(const void* address, int32 width, int32 height) requires (t_cfg.access == ACCESS_READONLY)
    {
        setBuffer(address);
        setResolution(width, height);
    }

    template <typename T, TextureConfiguration t_cfg>
    Texture2D<T, t_cfg>::Texture2D(void* address, int32 width, int32 height) requires (t_cfg.access == ACCESS_READWRITE)
    {
        setBuffer(address);
        setResolution(width, height);
    }

    template <typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::setBuffer(const void* address) requires (t_cfg.access == ACCESS_READONLY)
    {
        buffer = reinterpret_cast<BufferPointer>(address);
    }

    template <typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::setBuffer(void* address) requires (t_cfg.access == ACCESS_READWRITE)
    {
        buffer = reinterpret_cast<BufferPointer>(address);
    }

    template <typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::setResolution(int32 width, int32 height)
    {
        assert(width >= 0 && height >= 0);
        texture_width = width;
        texture_height = height;
    }

    template<typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::BufferPointer Texture2D<T, t_cfg>::getBuffer() const
    {
        return buffer;
    }

    template<typename T, TextureConfiguration t_cfg>
    int32 Texture2D<T, t_cfg>::getWidth() const
    {
        return texture_width;
    }

    template<typename T, TextureConfiguration t_cfg>
    int32 Texture2D<T, t_cfg>::getHeight() const
    {
        return texture_height;
    }

    template <typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::BufferPosition Texture2D<T, t_cfg>::pixelNumToBufferPosition(int32 pixel_num) const
    {
        // Compute position address and alignment inside buffer, based on interal format.
        BufferPosition position;
        if constexpr (t_cfg.format == FORMAT_RGB888 || t_cfg.format == FORMAT_RGB565 ||
                      t_cfg.format == FORMAT_RGBA4444 || t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 ||
                      t_cfg.format == FORMAT_R8 || t_cfg.format == FORMAT_DEPTH) {
            position.address = buffer + pixel_num;
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            position.address = buffer + (pixel_num >> 1);
            position.alignment = pixel_num & 0x1; // is RGB444_ALIGNMENT_EVEN or RGB444_ALIGNMENT_ODD
        }
        return position;
    }

    template<typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::BufferPosition Texture2D<T, t_cfg>::getWrappedBufferPosition(int32 x, int32 y) const
    {
        // Modify sample coordinates, based on wrapmode.
        if constexpr (t_cfg.wrapmode == WRAPMODE_CLAMPING) {
            x = std::clamp(x, static_cast<int32>(0), texture_width - 1);
            y = std::clamp(y, static_cast<int32>(0), texture_height - 1);
        }
        else if constexpr (t_cfg.wrapmode == WRAPMODE_TILING) {
            x = std::abs(x % texture_width);
            y = std::abs(y % texture_height);
        }

        return pixelNumToBufferPosition(x + texture_width * y);
    }

    template<typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::moveBufferPositionRight(BufferPosition& position)
    {
        // Move position address and alignment inside buffer by one pixel, based on interal format.
        if constexpr (t_cfg.format == FORMAT_RGB888 || t_cfg.format == FORMAT_RGB565 ||
                      t_cfg.format == FORMAT_RGBA4444 || t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 ||
                      t_cfg.format == FORMAT_R8 || t_cfg.format == FORMAT_DEPTH) {
            position.address += 1;
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            if (position.alignment == RGB444_ALIGNMENT_ODD) {
                position.address += 1;
                position.alignment = RGB444_ALIGNMENT_EVEN;
            }
            else { // == RGB444_ALIGNMENT_EVEN
                position.alignment = RGB444_ALIGNMENT_ODD;
            }
        }
    }

    template <typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::moveBufferPositionDown(BufferPosition& position)
    {
        // Move position address and alignment inside buffer by one pixel row, based on interal format.
        if constexpr (t_cfg.format == FORMAT_RGB888 || t_cfg.format == FORMAT_RGB565 || t_cfg.format == FORMAT_RGB444 ||
                      t_cfg.format == FORMAT_RGBA4444 || t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 ||
                      t_cfg.format == FORMAT_R8 || t_cfg.format == FORMAT_DEPTH) {
            position.address += texture_width;
        }
    }

    template<typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::ExternalType Texture2D<T, t_cfg>::readPixelAt(int32 x, int32 y) const
    {
        return readPixelAt(getWrappedBufferPosition(x, y));
    }

    template<typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::ExternalType Texture2D<T, t_cfg>::readPixelAt(Vector2<T> uv) const
    {
        return readPixelAt(std::lround(uv.x * texture_width - static_cast<T>(0.5)), std::lround(uv.y * texture_height - static_cast<T>(0.5)));
    }

    template<typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::ExternalType Texture2D<T, t_cfg>::readPixelAt(BufferPosition position) const
    {
        // Depth-format is stored as ExternalType, return pixel directly.
        if constexpr (t_cfg.format == FORMAT_DEPTH) {
            return *position.address;
        }

        // Read pixel from buffer, stored as InternalType, and convert it to WorkingType, based on texture format.
        WorkingType pixel;
        if constexpr (t_cfg.format == FORMAT_RGB888) {
            assert(verifyBufferPosition(position));
            pixel.r = static_cast<uint32>(position.address->r);
            pixel.g = static_cast<uint32>(position.address->g);
            pixel.b = static_cast<uint32>(position.address->b);
        }
        else if constexpr (t_cfg.format == FORMAT_RGB565) {
            assert(verifyBufferPosition(position));
            auto abc = *(reinterpret_cast<const uint16*>(position.address));
            pixel.r = static_cast<uint32>((*position.address & 0xF800) >> 11);
            pixel.g = static_cast<uint32>((*position.address & 0x07E0) >> 5);
            pixel.b = static_cast<uint32>(*position.address & 0x001F);
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            assert(verifyBufferPosition(position));
            if (position.alignment == RGB444_ALIGNMENT_ODD) {
                pixel.r = static_cast<uint32>(position.address->g & 0x0F);
                pixel.g = static_cast<uint32>(position.address->b >> 4);
                pixel.b = static_cast<uint32>(position.address->b & 0x0F);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                pixel.r = static_cast<uint32>(position.address->r >> 4);
                pixel.g = static_cast<uint32>(position.address->r & 0x0F);
                pixel.b = static_cast<uint32>(position.address->g >> 4);
            }
        }
        else if constexpr (t_cfg.format == FORMAT_RGBA4444) {
            assert(verifyBufferPosition(position));
            pixel.r = static_cast<uint32>((*position.address & 0xF000) >> 12);
            pixel.g = static_cast<uint32>((*position.address & 0x0F00) >> 8);
            pixel.b = static_cast<uint32>((*position.address & 0x00F0) >> 4);
            pixel.a = static_cast<uint32>(*position.address & 0x000F);
        }
        else if constexpr (t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 || t_cfg.format == FORMAT_R8) {
            pixel = static_cast<uint32>(*position.address);
        }

        // Apply pixel channel swizzle.
        if constexpr (t_cfg.swizzle == SWIZZLE_BGR) {
            uint8 temp = pixel.r;
            pixel.r = pixel.b;
            pixel.b = temp;
        }

        // Convert pixel from WorkingType to ExternalType.
        if constexpr(t_cfg.type == TYPE_INTEGER) {
            return pixel;
        }
        else if constexpr (t_cfg.type == TYPE_DECIMAL) {
            return static_cast<ExternalType>(pixel);
        }
        else if constexpr (t_cfg.type == TYPE_NORMALIZED) {
            constexpr auto inv_max_values = ExternalType{1.0} / static_cast<ExternalType>(TextureInternal<t_cfg.format, T>::MaxValue);
            return static_cast<ExternalType>(pixel) * inv_max_values;
        }
    }

    template<typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::drawPixelAt(int32 x, int32 y, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE)
    {
        drawPixelAt(getWrappedBufferPosition(x, y), value);
    }

    template<typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::drawPixelAt(Vector2<T> uv, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE)
    {
        drawPixelAt(std::lround(uv.x * texture_width - static_cast<T>(0.5)), std::lround(uv.y * texture_height - static_cast<T>(0.5)), value);
    }

    template<typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::drawPixelAt(BufferPosition position, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE)
    {
        // Depth-format is stored as ExternalType, store pixel directly.
        if constexpr (t_cfg.format == FORMAT_DEPTH) {
            *position.address = value;
            return;
        }

        // Convert pixel from ExternalType to WorkingType.
        WorkingType pixel;
        if constexpr(t_cfg.type == TYPE_INTEGER) {
            pixel = value;
        }
        else if constexpr (t_cfg.type == TYPE_DECIMAL) {
            pixel = static_cast<WorkingType>(value);
        }
        else if constexpr (t_cfg.type == TYPE_NORMALIZED) {
            constexpr auto max_values = static_cast<ExternalType>(TextureInternal<t_cfg.format, T>::MaxValue);
            pixel = static_cast<WorkingType>(value * max_values);
        }

        // Apply pixel channel swizzle.
        if constexpr (t_cfg.swizzle == SWIZZLE_BGR) {
            uint8 temp = pixel.r;
            pixel.r = pixel.b;
            pixel.b = temp;
        }

        // Convert pixel from WorkingType to InternalType, and store in buffer, based on texture format.
        if constexpr (t_cfg.format == FORMAT_RGB888) {
            assert(verifyBufferPosition(position));
            position.address->r = static_cast<uint8>(pixel.r);
            position.address->g = static_cast<uint8>(pixel.g);
            position.address->b = static_cast<uint8>(pixel.b);
        }
        else if constexpr (t_cfg.format == FORMAT_RGB565) {
            assert(verifyBufferPosition(position));
            *position.address = static_cast<uint16>(pixel.r << 11);
            *position.address |= static_cast<uint16>(pixel.g << 5);
            *position.address |= static_cast<uint16>(pixel.b);
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            assert(verifyBufferPosition(position));
            if (position.alignment == RGB444_ALIGNMENT_ODD) {
                // Pixel rgb spans: XXXrgb of position.address' RRGGBB (one letter occupies 4 byte).
                position.address->g &= 0xF0;
                position.address->g |= static_cast<uint8>(pixel.r);
                position.address->b = static_cast<uint8>(pixel.g << 4);
                position.address->b |= static_cast<uint8>(pixel.b);
            }
            else { // == RGB444_ALIGNMENT_EVEN
                // Pixel rgb spans: rgbXXX of position.address' RRGGBB (one letter occupies 4 byte).
                position.address->r = static_cast<uint8>(pixel.r << 4);
                position.address->r |= static_cast<uint8>(pixel.g);
                position.address->g &= 0x0F;
                position.address->g |= static_cast<uint8>(pixel.b << 4);
            }
        }
        else if constexpr (t_cfg.format == FORMAT_RGBA4444) {
            assert(verifyBufferPosition(position));
            *position.address = static_cast<uint16>(pixel.r << 12);
            *position.address |= static_cast<uint16>(pixel.g << 8);
            *position.address |= static_cast<uint16>(pixel.b << 4);
            *position.address |= static_cast<uint16>(pixel.a);
        }
        else if constexpr (t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 || t_cfg.format == FORMAT_R8) {
            *position.address = static_cast<InternalType>(pixel);
        }
    }

    template<typename T, TextureConfiguration t_cfg>
    void Texture2D<T, t_cfg>::clearBuffer(const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE)
    {
        // Fill buffer with constant value, based on texture format.
        drawPixelAt(0, 0, value);
        const InternalType internal_value = *buffer;
        const int32 buffer_size = texture_width * texture_height;
        BufferPointer pixel = buffer;
        for (int32 i = 1; i < buffer_size; ++i) {
            ++pixel;
            *pixel = internal_value;
        }
    }

    template <typename T, TextureConfiguration t_cfg>
    typename Texture2D<T, t_cfg>::ExternalType Texture2D<T, t_cfg>::samplePixelAt(Vector2<T> uv) const
    {
        // Bilinear interpolate the four closest pixels around sample point.
        uv.x *= texture_width;
        uv.y *= texture_height;
        uv -= Vector2<T>(0.5);
        T left_x = std::floor(uv.x);
        T right_x = std::ceil(uv.x);
        T top_y = std::floor(uv.y);
        T bottom_y = std::ceil(uv.y);
        ExternalType top_left = readPixelAt(static_cast<int32>(left_x), static_cast<int32>(top_y));
        ExternalType top_right = readPixelAt(static_cast<int32>(right_x), static_cast<int32>(top_y));
        ExternalType bottom_left = readPixelAt(static_cast<int32>(left_x), static_cast<int32>(bottom_y));
        ExternalType bottom_right = readPixelAt(static_cast<int32>(right_x), static_cast<int32>(bottom_y));
        T alpha_x = uv.x - left_x;
        T alpha_y = uv.y - top_y;
        ExternalType top = top_left * (static_cast<T>(1.0) - alpha_x) + top_right * alpha_x;
        ExternalType bottom = bottom_left * (static_cast<T>(1.0) - alpha_x) + bottom_right * alpha_x;
        return top * (static_cast<T>(1.0) - alpha_y) + bottom * alpha_y;
    }

    template<typename T, TextureConfiguration t_cfg>
    bool Texture2D<T, t_cfg>::saveToPPMImage(const std::string& file_name) const
    {
        // Check parameters and open file.
        if (!buffer || texture_width < 1 || texture_height < 1 || texture_width > 5000 || texture_height > 5000) {
            assert(false && "Invalid parameters when trying to save framebuffer to ppm file!");
            return false;
        }
        std::ofstream file(file_name + ".ppm", std::ios_base::out | std::ios_base::binary);
        if (!file.is_open()) {
            assert(false && "Image file could not be opened!");
            return false;
        }

        // Insert ppm image header into file stream, based on texture format.
        if constexpr (t_cfg.format == FORMAT_RGB888) {
            file << "P3\n" << texture_width << ' ' << texture_height << "\n255\n";
        }
        else if constexpr (t_cfg.format == FORMAT_RGB565) {
            file << "P3\n" << texture_width << ' ' << texture_height << "\n31\n";
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            file << "P3\n" << texture_width << ' ' << texture_height << "\n15\n";
        }
        else if constexpr (t_cfg.format == FORMAT_RGBA4444) {
            file << "P3\n" << texture_width << ' ' << texture_height << "\n15\n";
        }
        else if constexpr (t_cfg.format == FORMAT_R8) {
            file << "P2\n" << texture_width << ' ' << texture_height << "\n255\n";
        }
        else if constexpr (t_cfg.format == FORMAT_R16) {
            file << "P2\n" << texture_width << ' ' << texture_height << "\n65535\n";
        }
        else if constexpr (t_cfg.format == FORMAT_R32) {
            file << "P2\n" << texture_width << ' ' << texture_height << "\n65535\n";
        }
        else if constexpr (t_cfg.format == FORMAT_DEPTH) {
            file << "P2\n" << texture_width << ' ' << texture_height << "\n65535\n";
        }

        // Write framebuffer to file.
        for (int32 y = 0; y < texture_height; ++y) {
            for (int32 x = 0; x < texture_width; ++x) {
                ExternalType value = readPixelAt(x, y);
                if constexpr (t_cfg.format == FORMAT_DEPTH) {
                    value = std::clamp(value, static_cast<T>(0.0), static_cast<T>(1.0));
                    file << std::to_string(static_cast<uint32>(value * static_cast<T>(65535.0))) << "\n";
                    continue;
                }

                // Convert pixel from ExternalType to WorkingType.
                WorkingType pixel;
                if constexpr(t_cfg.type == TYPE_INTEGER) {
                    pixel = value;
                }
                else if constexpr (t_cfg.type == TYPE_DECIMAL) {
                    pixel = static_cast<WorkingType>(value);
                }
                else if constexpr (t_cfg.type == TYPE_NORMALIZED) {
                    constexpr auto max_values = static_cast<ExternalType>(TextureInternal<t_cfg.format, T>::MaxValue);
                    pixel = static_cast<WorkingType>(value * max_values);
                }

                // Insert pixel into file stream, based on texture format and swizzle.
                if constexpr (t_cfg.format == FORMAT_RGB888) {
                    file << std::to_string(pixel.r) << " " << std::to_string(pixel.g) << " " << std::to_string(pixel.b) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_RGB565) {
                    file << std::to_string(pixel.r) << " " << std::to_string(pixel.g >> 1) << " " << std::to_string(pixel.b) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_RGB444) {
                    file << std::to_string(pixel.r) << " " << std::to_string(pixel.g) << " " << std::to_string(pixel.b) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_RGBA4444) {
                    file << std::to_string(pixel.r) << " " << std::to_string(pixel.g) << " " << std::to_string(pixel.b) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_R8) {
                    file << std::to_string(pixel) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_R16) {
                    file << std::to_string(pixel) << "\n";
                }
                else if constexpr (t_cfg.format == FORMAT_R32) {
                    file << std::to_string(pixel >> 16) << "\n";
                }
            }
        }
        return true;
    }

    template<typename T, TextureConfiguration t_cfg>
    bool Texture2D<T, t_cfg>::verifyBufferPosition(BufferPosition position) const
    {
        // Check if position points to valid pixel inside buffer, based on texture format.
        if (!buffer || position.address < buffer)
            return false;
        int32 num_pixels = texture_width * texture_height;
        if constexpr (t_cfg.format == FORMAT_RGB888 || t_cfg.format == FORMAT_RGB565 ||
                      t_cfg.format == FORMAT_RGBA4444 || t_cfg.format == FORMAT_R32 || t_cfg.format == FORMAT_R16 ||
                      t_cfg.format == FORMAT_R8 || t_cfg.format == FORMAT_DEPTH) {
            if (position.address >= buffer + num_pixels)
                return false;
        }
        else if constexpr (t_cfg.format == FORMAT_RGB444) {
            if (position.address > buffer + (num_pixels >> 1))
                return false;
            if (texture_width % 2) {
                if (position.address == buffer + (num_pixels >> 1) && position.alignment == RGB444_ALIGNMENT_ODD)
                    return false;
            }
        }
        return true;
    }

} // namespace MicroRenderer
