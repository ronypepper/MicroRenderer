//
// Created by pusdo on 03/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/ShaderConfiguration.h"
#include "MicroRenderer/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Interpolation.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template <typename T, ShaderConfiguration t_cfg, template <typename, ShaderConfiguration> class VertexShader,
          template <typename, ShaderConfiguration> class TriangleAssembler,
          template <typename, ShaderConfiguration> class FragmentShader>
class BaseShaderProgram
{
public:
    using VertexShader_type = VertexShader<T, t_cfg>;
    using TriangleAssembler_type = TriangleAssembler<T, t_cfg>;
    using FragmentShader_type = FragmentShader<T, t_cfg>;
    USE_SHADER_INTERFACE(VertexShader_type::ShaderInterface);

    static_assert(t_cfg.projection < NUM_PROJECTION_TYPES, "ShaderProgram: Invalid projection type in configuration!");
    static_assert(t_cfg.culling < NUM_CULLING_MODES, "ShaderProgram: Invalid culling mode in configuration!");
    static_assert(t_cfg.clipping < NUM_CLIPPING_MODES, "ShaderProgram: Invalid clipping mode in configuration!");
    static_assert(t_cfg.depth_test < NUM_DEPTH_TEST_MODES, "ShaderProgram: Invalid depth test mode in configuration!");
    static_assert(t_cfg.shading < NUM_SHADING_MODES, "ShaderProgram: Invalid shading mode in configuration!");
    static_assert(t_cfg.projection != PERSPECTIVE || t_cfg.clipping == CLIP_AT_NEAR_PLANE,
                  "ShaderProgram: Perspective projection requires clipping at near plane to be enabled!");
    static_assert(t_cfg.depth_test == DEPTH_TEST_ENABLED || t_cfg.shading == SHADING_ENABLED,
                  "ShaderProgram: At least depth test or shading must be enabled!");
    static_assert(std::is_same_v<ShaderInterface, typename TriangleAssembler_type::ShaderInterface> &&
                  std::is_same_v<ShaderInterface, typename FragmentShader_type::ShaderInterface>,
                  "ShaderProgram: ShaderInterface type used in VertexShader, TriangleAssembler and FragmentShader must match!");

    static constexpr ShaderConfiguration configuration = t_cfg;
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

    void shadeVertex(VertexData vertex)
    {
        VertexShader_type::shadeVertex(uniform_data, vertex);
    }

    void interpolateVertices(VertexData from, VertexData to, VertexSource* new_src, VertexBuffer* new_buf)
    {
        // Get clip position based on projection mode.
        const Vector3<T>& from_clip_pos = from.buffer->getClipPosition();
        const Vector3<T>& to_clip_pos = to.buffer->getClipPosition();

        // Compute clipped position of new vertex.
        const T to_factor = from_clip_pos.z / (from_clip_pos.z - to_clip_pos.z);
        const T from_factor = static_cast<T>(1.0) - to_factor;
        const T new_clip_x = interpolateLinearly(from_clip_pos.x, to_clip_pos.x, from_factor, to_factor);
        const T new_clip_y = interpolateLinearly(from_clip_pos.y, to_clip_pos.y, from_factor, to_factor);

        // Set clipped and screen position of new vertex based on projection mode.
        if constexpr (t_cfg.projection == PERSPECTIVE) {
            new_buf->clip_position = {new_clip_x, new_clip_y, static_cast<T>(0.0), inv_near_plane};
            new_buf->screen_position = {new_clip_x * inv_near_plane, new_clip_y * inv_near_plane, static_cast<T>(0.0)};
        }
        else if constexpr (t_cfg.projection == ORTHOGRAPHIC) {
            new_buf->screen_position = {new_clip_x, new_clip_y, static_cast<T>(0.0)};
        }

        TriangleAssembler_type::interpolateVertices(uniform_data, from, to, new_src, new_buf, from_factor, to_factor);
    }

    void setupTriangle(uint32 tri_idx, VertexData v1, VertexData v2, VertexData v3, TriangleBuffer* triangle,
                       int32 start_x, int32 start_y)
    {
        const Vector3<T>& pos_1 = v1.buffer->screen_position;
        const Vector3<T>& pos_2 = v2.buffer->screen_position;
        const Vector3<T>& pos_3 = v3.buffer->screen_position;
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

        TriangleAssembler_type::setupTriangle(uniform_data, tri_idx, v1, v2, v3, triangle, v1_offset, bc_incs);
    }

    template<IncrementationMode mode>
    void interpolateAttributes(TriangleBuffer* triangle, int32 offset = 1)
    {
        if constexpr(t_cfg.depth_test == DEPTH_TEST_ENABLED) {
            // Interpolate depth.
            triangle->depth.template increment<mode>(offset);
        }

        FragmentShader_type::template interpolateAttributes<mode>(uniform_data, triangle, offset);
    }

    ShaderOutput computeColor(TriangleBuffer* triangle)
    {
        return FragmentShader_type::computeColor(uniform_data, triangle);
    }
private:
    UniformData uniform_data;

    InverseNearPlaneType inv_near_plane;
};

} // namespace MicroRenderer
