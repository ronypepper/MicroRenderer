//
// Created by pusdo on 15/08/2024.
//

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector3.h"
#include "TextureConfiguration.h"
#include "TextureTypes.h"
#include <string>

namespace MicroRenderer {

template<typename T, TextureConfig t_cfg>
class Texture2D
{
    static_assert(t_cfg.access < NUM_ACCESS_MODES, "Invalid access mode in texture configuration!");
    static_assert(t_cfg.format < NUM_INTERNAL_FORMATS, "Invalid internal format in texture configuration!");
    static_assert(t_cfg.swizzle < NUM_SWIZZLES, "Invalid swizzle in texture configuration!");
    static_assert(t_cfg.type < NUM_EXTERNAL_TYPES, "Invalid external type in texture configuration!");
    static_assert(t_cfg.wrapmode < NUM_WRAPMODES, "Invalid wrapmode in texture configuration!");
public:
    // Representation of texture values in memory.
    // I.e. Vectorx<uint8>, uint8, Vectorx<float>, float, uint16 ...
    using InternalType = typename TextureInternal<t_cfg.format, T>::InternalType;

    // Intermediate representation of texture values in methods after reading from / before writing to texture.
    // One of Vectorx<uint32>, uint32
    using WorkingType = typename TextureInternal<t_cfg.format, T>::template PixelType<uint32>;

    // Representation of texture values in Texture2D's methods.
    // One of Vectorx<uint32>, Vectorx<float>, uint32, float
    using ExternalValue = typename TextureExternal<t_cfg.type>::type;
    using ExternalType = typename TextureInternal<t_cfg.format, T>::template PixelType<ExternalValue>;

    using BufferPointer = std::conditional_t<t_cfg.access == ACCESS_READWRITE, InternalType*, const InternalType*>;
    struct BufferPosition
    {
        BufferPointer address;
        uint32 alignment;
    };

    Texture2D() = default;

    Texture2D(BufferPointer address, int32 width, int32 height);

    void changeBuffer(BufferPointer address, int32 width, int32 height);

    BufferPointer getBuffer() const;

    int32 getWidth() const;

    int32 getHeight() const;

    BufferPosition getBufferPositionAt(int32 x, int32 y) const;

    void moveBufferPositionRight(BufferPosition& position);

    void moveBufferPositionDown(BufferPosition& position);

    ExternalType readPixelAt(int32 x, int32 y) const;

    ExternalType readPixelAt(Vector2<T> uv) const;

    ExternalType readPixelAt(BufferPosition position) const;

    void drawPixelAt(int32 x, int32 y, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE);

    void drawPixelAt(Vector2<T> uv, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE);

    void drawPixelAt(BufferPosition position, const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE);

    void clearBuffer(const ExternalType& value) requires(t_cfg.access == ACCESS_READWRITE);

    ExternalType samplePixelAt(Vector2<T> uv) const;

    void saveToPPMImage(const std::string& file_name) const;

    bool verifyBufferPosition(BufferPosition position) const;

    // Texture width.
    int32 texture_width = 0;

    // Texture height.
    int32 texture_height = 0;

    // Pointer to buffer memory.
    BufferPointer buffer = nullptr;
};

} // namespace MicroRenderer

#include "Texture2D.tpp"
