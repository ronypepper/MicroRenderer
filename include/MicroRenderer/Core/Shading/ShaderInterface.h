//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"
#include "MicroRenderer/Core/Textures/Texture2D.h"
#include "MicroRenderer/Math/Interpolation.h"

namespace MicroRenderer {

template<typename T>
struct BaseOrthographicVertexBuffer
{
    Vector3<T> screen_position;
};

template<typename T>
struct BasePerspectiveVertexBuffer
{
    Vector4<T> clip_position;
    Vector3<T> screen_position;

};

template<typename T>
struct BaseDepthTriangleBuffer
{
    TriangleAttribute<T, T> depth;
};

struct ShaderOutput
{
    TextureInternalFormat format;
    TextureExternalType type;
};

struct TriangleIndices
{
    uint16 vertex_1_idx;
    uint16 vertex_2_idx;
    uint16 vertex_3_idx;
};

template <typename T, ShaderOutput output, template <typename> class GlobalData, template <typename> class InstanceData,
          template <typename> class VertexSource, template <typename> class VertexBuffer, template <typename> class TriangleBuffer>
class BaseShaderInterface
{
    // Deduce return type of fragment shader's computeColor method. ShaderOutput becomes equal to a Texture2D's
    // ExternalType with same TextureInternalFormat and TextureExternalType configuration.
    using ExternalValue = typename TextureExternal<output.type, T>::type;
    using ExternalType = typename TextureInternal<output.format, T>::template PixelType<ExternalValue>;
public:
    static constexpr ShaderOutput shader_output = output;

    using Value_type = T;
    using ShaderOutput_type = ExternalType;
    using GlobalData_type = GlobalData<T>;
    using InstanceData_type = InstanceData<T>;
    using VertexSource_type = VertexSource<T>;
    using VertexBuffer_type = VertexBuffer<T>;
    using TriangleBuffer_type = TriangleBuffer<T>;

    struct UniformData
    {
        const GlobalData<T>* global;
        const InstanceData<T>* instance;
    };
    struct VertexData
    {
        const VertexSource<T>* source;
        VertexBuffer<T>* buffer;
    };
    struct ModelData
    {
        uint16 num_vertices;
        uint16 num_triangles;
        const VertexSource<T>* vertices;
        const TriangleIndices* indices;
    };
};

#define USE_SHADER_INTERFACE(Interface) \
    using ShaderInterface = Interface; \
    using ShaderOutput = typename Interface::ShaderOutput_type; \
    using GlobalData = typename Interface::GlobalData_type; \
    using InstanceData = typename Interface::InstanceData_type; \
    using UniformData = typename Interface::UniformData; \
    using VertexSource = typename Interface::VertexSource_type; \
    using VertexBuffer = typename Interface::VertexBuffer_type; \
    using VertexData = typename Interface::VertexData; \
    using TriangleBuffer = typename Interface::TriangleBuffer_type; \
    using ModelData = typename Interface::ModelData;

} // namespace MicroRenderer
