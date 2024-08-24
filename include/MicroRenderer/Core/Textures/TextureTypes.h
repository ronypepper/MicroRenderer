//
// Created by pusdo on 18/08/2024.
//

#pragma once
#include "TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"

namespace MicroRenderer {

template<TextureInternalFormat internal_format, typename T>
struct TextureInternal
{
};

template<typename T>
struct TextureInternal<FORMAT_RGB888, T>
{
    using InternalType = Vector3<uint8>;
    template<typename U>
    using PixelType = Vector3<U>;
    static constexpr PixelType<uint32> MaxValue = {255, 255, 255};
};

template<typename T>
struct TextureInternal<FORMAT_RGB565, T>
{
    using InternalType = uint16;
    template<typename U>
    using PixelType = Vector3<U>;
    static constexpr PixelType<uint32> MaxValue = {31, 63, 31};
};

template<typename T>
struct TextureInternal<FORMAT_RGB444, T>
{
    using InternalType = Vector3<uint8>;
    template<typename U>
    using PixelType = Vector3<U>;
    static constexpr PixelType<uint32> MaxValue = {15, 15, 15};
};

template<typename T>
struct TextureInternal<FORMAT_RGBA4444, T>
{
    using InternalType = uint16;
    template<typename U>
    using PixelType = Vector4<U>;
    static constexpr PixelType<uint32> MaxValue = {15, 15, 15, 15};
};

template<typename T>
struct TextureInternal<FORMAT_R8, T>
{
    using InternalType = uint8;
    template<typename U>
    using PixelType = U;
    static constexpr PixelType<uint32> MaxValue = 255;
};

template<typename T>
struct TextureInternal<FORMAT_R16, T>
{
    using InternalType = uint16;
    template<typename U>
    using PixelType = U;
    static constexpr PixelType<uint32> MaxValue = 65535;
};

template<typename T>
struct TextureInternal<FORMAT_R32, T>
{
    using InternalType = uint32;
    template<typename U>
    using PixelType = U;
    static constexpr PixelType<uint32> MaxValue = 4294967295;
};

template<typename T>
struct TextureInternal<FORMAT_DEPTH, T>
{
    using InternalType = T;
    template<typename U>
    using PixelType = U;
};


template<TextureExternalType external_type>
struct TextureExternal
{
};

template<>
struct TextureExternal<TYPE_INTEGER>
{
    using type = uint32;
};

template<>
struct TextureExternal<TYPE_DECIMAL>
{
    using type = float;
};

template<>
struct TextureExternal<TYPE_NORMALIZED>
{
    using type = float;
};

} // namespace MicroRenderer
