//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class SimpleContoursTriangleAssembler : public BaseTriangleAssembler<T, t_cfg, SimpleContoursShaderInterface, SimpleContoursTriangleAssembler>
{
public:
    using ShaderInterface_type = SimpleContoursShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void interpolateVertices_implementation(UniformData uniform, VertexData from, VertexData to,
                                                   VertexSource* new_src, VertexBuffer* new_buf, T from_factor,
                                                   T to_factor)
    {
        new_src->position = interpolateLinearly(from.source->position, to.source->position, from_factor, to_factor);
    }

    static void setupTriangle_implementation(UniformData uniform, uint32 tri_idx, VertexData v1, VertexData v2,
                                             VertexData v3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // Compute shading based on orientation of triangle normal to sun direction for a simple contour.
        const Vector3<T>& normal = uniform.instance->triangle_normals[tri_idx];
        T shade_factor = uniform.instance->towards_sun_dir_model_space.dot(normal);
        shade_factor = std::max(static_cast<T>(0.2), (shade_factor + static_cast<T>(1.0)) * static_cast<T>(0.5));
        triangle->shading = uniform.instance->color * shade_factor;
    }
};

} // namespace MicroRenderer
