//
// Created by pusdo on 03/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Interpolation.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

enum ProjectionType : uint32
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
    NUM_PROJECTION_TYPES
};

enum CullingMode : uint32
{
    CULL_AT_SCREEN_BORDER,
    NO_CULLING,
    NUM_CULLING_MODES
};

enum ClippingMode : uint32
{
    CLIP_AT_NEAR_PLANE,
    NO_CLIPPING,
    NUM_CLIPPING_MODES
};

enum DepthTestMode : uint32
{
    DEPTH_TEST_ENABLED,
    DEPTH_TEST_DISABLED,
    NUM_DEPTH_TEST_MODES
};

enum ShadingMode : uint32
{
    SHADING_ENABLED,
    SHADING_DISABLED,
    NUM_SHADING_MODES
};

struct ShaderProgramConfig
{
    ProjectionType projection;
    CullingMode culling;
    ClippingMode clipping;
    DepthTestMode depth_test;
    ShadingMode shading;
};

template <typename T, ShaderProgramConfig t_cfg, template <typename> class
          VertexShader, template <typename> class TriangleAssembler, template <typename> class FragmentShader>
class BaseShaderProgram
{
public:
    USE_SHADER_INTERFACE(VertexShader<T>::ShaderInterface);

    static_assert(t_cfg.projection < NUM_PROJECTION_TYPES, "ShaderProgram: Invalid projection type in configuration!");
    static_assert(t_cfg.culling < NUM_CULLING_MODES, "ShaderProgram: Invalid culling mode in configuration!");
    static_assert(t_cfg.clipping < NUM_CLIPPING_MODES, "ShaderProgram: Invalid clipping mode in configuration!");
    static_assert(t_cfg.depth_test < NUM_DEPTH_TEST_MODES, "ShaderProgram: Invalid depth test mode in configuration!");
    static_assert(t_cfg.shading < NUM_SHADING_MODES, "ShaderProgram: Invalid shading mode in configuration!");
    static_assert(t_cfg.projection != PERSPECTIVE || t_cfg.clipping == CLIP_AT_NEAR_PLANE,
                  "ShaderProgram: Perspective projection requires clipping at near plane to be enabled!");
    static_assert(t_cfg.depth_test == DEPTH_TEST_ENABLED || t_cfg.shading == SHADING_ENABLED,
                  "ShaderProgram: At least depth test or shading must be enabled!");
    static_assert(std::is_same_v<ShaderInterface, typename TriangleAssembler<T>::ShaderInterface> &&
                  std::is_same_v<ShaderInterface, typename FragmentShader<T>::ShaderInterface>,
                  "ShaderProgram: ShaderInterface type used in VertexShader, TriangleAssembler and FragmentShader must match!");
    static_assert(t_cfg.projection != ORTHOGRAPHIC || std::is_base_of_v<BaseOrthographicVertexBuffer<T>, VertexBuffer>,
                  "ShaderProgram: VertexBuffer must inherit from BaseOrthographicVertexBuffer when using orthographic projection!");
    static_assert(t_cfg.projection != PERSPECTIVE || std::is_base_of_v<BasePerspectiveVertexBuffer<T>, VertexBuffer>,
                  "ShaderProgram: VertexBuffer must inherit from BasePerspectiveVertexBuffer when using perspective projection!");
    static_assert(t_cfg.depth_test != DEPTH_TEST_ENABLED || std::is_base_of_v<BaseDepthTriangleBuffer<T>, TriangleBuffer>,
                  "ShaderProgram: TriangleBuffer must inherit from BaseDepthTriangleBuffer when depth test is enabled!");

    static constexpr ShaderProgramConfig configuration = t_cfg;
    using InverseNearPlaneType = std::conditional_t<t_cfg.projection == PERSPECTIVE, T, std::monostate>;

    void setGlobalData(const GlobalData* data)
    {
        uniform_data.global = data;
    }
    void setInstanceData(const InstanceData* data)
    {
        uniform_data.instance = data;
    }
    void setNearPlane(T distance) requires(t_cfg.projection == PERSPECTIVE)
    {
        inv_near_plane = static_cast<T>(1.0) / distance;
    }

    void transformVertex(VertexData vertex)
    {
        VertexShader<T>::transformVertex(uniform_data, vertex);
    }

    void shadeVertex(VertexData vertex)
    {
        VertexShader<T>::shadeVertex(uniform_data, vertex);
    }

    void interpolateVertices(VertexData from, VertexData to, VertexSource* new_src, VertexBuffer* new_buf)
    {
        if constexpr (t_cfg.projection == ORTHOGRAPHIC) {
            const Vector3<T>& from_clip_pos = from.buffer->screen_position;
            const Vector3<T>& to_clip_pos = to.buffer->screen_position;
            const T to_factor = from_clip_pos.z / (from_clip_pos.z - to_clip_pos.z);
            const T from_factor = static_cast<T>(1.0) - to_factor;
            const T new_clip_x = interpolateLinearly(from_clip_pos.x, to_clip_pos.x, from_factor, to_factor);
            const T new_clip_y = interpolateLinearly(from_clip_pos.y, to_clip_pos.y, from_factor, to_factor);
            new_buf->screen_position = {new_clip_x, new_clip_y, static_cast<T>(0.0)};
            TriangleAssembler<T>::interpolateVertices(uniform_data, from, to, new_src, new_buf, from_factor, to_factor);
        }
        else if constexpr (t_cfg.projection == PERSPECTIVE) {
            const Vector4<T>& from_clip_pos = from.buffer->clip_position;
            const Vector4<T>& to_clip_pos = to.buffer->clip_position;
            const T to_factor = from_clip_pos.z / (from_clip_pos.z - to_clip_pos.z);
            const T from_factor = static_cast<T>(1.0) - to_factor;
            const T new_clip_x = interpolateLinearly(from_clip_pos.x, to_clip_pos.x, from_factor, to_factor);
            const T new_clip_y = interpolateLinearly(from_clip_pos.y, to_clip_pos.y, from_factor, to_factor);
            new_buf->clip_position = {new_clip_x, new_clip_y, static_cast<T>(0.0), inv_near_plane};
            new_buf->screen_position = {new_clip_x * inv_near_plane, new_clip_y * inv_near_plane, static_cast<T>(0.0)};
            TriangleAssembler<T>::interpolateVertices(uniform_data, from, to, new_src, new_buf, from_factor, to_factor);
        }
    }

    void setupTriangle(VertexData vertex_1, VertexData vertex_2, VertexData vertex_3, TriangleBuffer* triangle,
                       int32 start_x, int32 start_y)
    {
        const Vector3<T>& pos_1 = vertex_1.buffer->screen_position;
        const Vector3<T>& pos_2 = vertex_2.buffer->screen_position;
        const Vector3<T>& pos_3 = vertex_3.buffer->screen_position;
        BarycentricIncrements<T> bc_incs;
        computeBarycentricIncrements(pos_1.getXY(), pos_2.getXY(), pos_3.getXY(), bc_incs);

        const Vector2<T> v1_offset = {start_x - pos_1.x, start_y - pos_1.y};

        if constexpr(t_cfg.depth_test == DEPTH_TEST_ENABLED) {
            // Setup interpolation of depth (reversed-z) over triangle.
            const T rev_z_1 = static_cast<T>(1.0) - pos_1.z;
            const T rev_z_2 = static_cast<T>(1.0) - pos_2.z;
            const T rev_z_3 = static_cast<T>(1.0) - pos_3.z;
            triangle->depth.initialize(rev_z_1, rev_z_2, rev_z_3, bc_incs, v1_offset);
        }

        TriangleAssembler<T>::setupTriangle(uniform_data, vertex_1, vertex_2, vertex_3, triangle, v1_offset, bc_incs);
    }

    template<IncrementationMode mode>
    void interpolateAttributes(TriangleBuffer* triangle, int32 offset = 1)
    {
        if constexpr(t_cfg.depth_test == DEPTH_TEST_ENABLED) {
            // Interpolate depth.
            triangle->depth.increment<mode>(offset);
        }

        FragmentShader<T>::template interpolateAttributes<mode>(uniform_data, triangle, offset);
    }

    ShaderOutput computeColor(TriangleBuffer* triangle)
    {
        return FragmentShader<T>::computeColor(uniform_data, triangle);
    }
private:
    UniformData uniform_data;

    InverseNearPlaneType inv_near_plane;
};

} // namespace MicroRenderer
