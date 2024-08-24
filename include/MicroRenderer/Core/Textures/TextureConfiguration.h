//
// Created by pusdo on 18/08/2024.
//

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"

namespace MicroRenderer {

enum TextureAccess : uint32
{
    ACCESS_READONLY,
    ACCESS_READWRITE,
    NUM_ACCESS_MODES
};

enum TextureInternalFormat : uint32
{
    FORMAT_RGB888,
    FORMAT_RGB565,
    FORMAT_RGB444,
    FORMAT_RGBA4444,
    FORMAT_R8,
    FORMAT_R16,
    FORMAT_R32,
    FORMAT_DEPTH,
    NUM_INTERNAL_FORMATS
};

enum TextureSwizzle : uint32
{
    SWIZZLE_NONE,
    SWIZZLE_BGR,
    NUM_SWIZZLES
};

enum TextureExternalType : uint32
{
    TYPE_INTEGER,
    TYPE_DECIMAL,
    TYPE_NORMALIZED,
    NUM_EXTERNAL_TYPES
};

enum TextureWrapmode : uint32
{
    WRAPMODE_NONE,
    WRAPMODE_CLAMPING,
    WRAPMODE_TILING,
    NUM_WRAPMODES
};

struct TextureConfig
{
    TextureAccess access;
    TextureInternalFormat format;
    TextureSwizzle swizzle;
    TextureExternalType type;
    TextureWrapmode wrapmode;
};

// Texture alignments.
constexpr uint32 RGB444_ALIGNMENT_EVEN = 0;
constexpr uint32 RGB444_ALIGNMENT_ODD = 1;

} // namespace MicroRenderer
