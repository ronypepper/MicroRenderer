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
    }

    static void setupTriangle_implementation(UniformData uniform, VertexData vertex_1, VertexData vertex_2,
                                             VertexData vertex_3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // Compute triangle normal.
        Vector3<T> d1 = vertex_2.source->model_position - vertex_1.source->model_position;
        Vector3<T> d2 = vertex_3.source->model_position - vertex_1.source->model_position;
        Vector3<T> normal = d1.cross(d2);
        normal.normalizeSafe();

        // Compute shading based on orientation of normal to sun direction.
        T shade_factor = std::max(static_cast<T>(0.0), uniform.instance->towards_sun_dir_model_space.dot(normal));
        triangle->shading = uniform.instance->color * (shade_factor + static_cast<T>(1.0)) * static_cast<T>(0.5);
    }
};

} // namespace MicroRenderer
