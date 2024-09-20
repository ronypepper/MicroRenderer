//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderConfiguration.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"
#include "MicroRenderer/Core/Textures/Texture2D.h"
#include "MicroRenderer/Math/Interpolation.h"

namespace MicroRenderer {

struct BaseInstanceData
{
    uint16 model_idx = 0;
};

template<typename T, ShaderConfiguration t_cfg>
struct BaseVertexBuffer
{
    std::conditional_t<t_cfg.projection == PERSPECTIVE, Vector4<T>, std::monostate> clip_position;
    Vector3<T> screen_position;

    void setPosition(const Vector4<T>& position)
    {
        if constexpr (t_cfg.projection == PERSPECTIVE) {
            clip_position = position;
        }
        else if constexpr (t_cfg.projection == ORTHOGRAPHIC) {
            screen_position = position.getXYZ();
        }
    }
    Vector3<T> getClipPosition() const
    {
        if constexpr (t_cfg.projection == PERSPECTIVE) {
            return clip_position.getXYZ();
        }
        else if constexpr (t_cfg.projection == ORTHOGRAPHIC) {
            return screen_position;
        }
    }
    void homogenizeVertex(T near_plane) requires(t_cfg.projection == PERSPECTIVE)
    {
        if (clip_position.w >= near_plane) {
            clip_position.w = static_cast<T>(1.0) / clip_position.w;
            screen_position = clip_position.getXYZ() * clip_position.w;
        }
    }
};

template<typename T, ShaderConfiguration t_cfg>
struct BaseTriangleBuffer
{
    std::conditional_t<t_cfg.depth_test == DEPTH_TEST_ENABLED, TriangleAttribute<T, T>, std::monostate> depth;
};

struct TriangleIndices
{
    uint16 vertex_1_idx;
    uint16 vertex_2_idx;
    uint16 vertex_3_idx;
};

template <typename T, ShaderConfiguration t_cfg, template <typename> class GlobalData,
          template <typename> class InstanceData, template <typename> class VertexSource,
          template <typename, ShaderConfiguration> class VertexBuffer,
          template <typename, ShaderConfiguration> class TriangleBuffer>
class BaseShaderInterface
{
    static_assert(std::is_base_of_v<BaseInstanceData, InstanceData<T>>,
                  "ShaderInterface: InstanceData must inherit from BaseInstanceData!");
    static_assert(std::is_base_of_v<BaseVertexBuffer<T, t_cfg>, VertexBuffer<T, t_cfg>>,
                  "ShaderInterface: VertexBuffer must inherit from BaseVertexBuffer!");
    static_assert(std::is_base_of_v<BaseTriangleBuffer<T, t_cfg>, TriangleBuffer<T, t_cfg>>,
                  "ShaderInterface: TriangleBuffer must inherit from BaseTriangleBuffer!");

    // Deduce return type of fragment shader's computeColor method. ShaderOutput becomes equal to a Texture2D's
    // ExternalType with same TextureInternalFormat and TextureExternalType configuration.
    using ExternalValue = typename TextureExternal<t_cfg.output.type, T>::type;
    using ExternalType = typename TextureInternal<t_cfg.output.format, T>::template PixelType<ExternalValue>;
public:
    //using Value_type = T;
    using ShaderOutput_type = ExternalType;
    using GlobalData_type = GlobalData<T>;
    using InstanceData_type = InstanceData<T>;
    using VertexSource_type = VertexSource<T>;
    using VertexBuffer_type = VertexBuffer<T, t_cfg>;
    using TriangleBuffer_type = TriangleBuffer<T, t_cfg>;

    struct UniformData
    {
        const GlobalData_type* global;
        const InstanceData_type* instance;
    };
    struct VertexData
    {
        const VertexSource_type* source;
        VertexBuffer_type* buffer;
    };
    struct ModelData
    {
        uint16 num_vertices;
        uint16 num_triangles;
        const VertexSource_type* vertices;
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
