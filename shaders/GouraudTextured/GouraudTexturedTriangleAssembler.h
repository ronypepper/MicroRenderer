//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/TriangleAssembler.h"
#include "GouraudTexturedShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class GouraudTexturedTriangleAssembler : public BaseTriangleAssembler<T, t_cfg, GouraudTexturedShaderInterface, GouraudTexturedTriangleAssembler>
{
public:
    using ShaderInterface_type = GouraudTexturedShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void interpolateVertices_implementation(UniformData uniform, VertexData from, VertexData to,
                                                   VertexSource* new_src, VertexBuffer* new_buf, T from_factor,
                                                   T to_factor)
    {
        new_src->uv_coordinates = interpolateLinearly(from.source->uv_coordinates, to.source->uv_coordinates, from_factor, to_factor);
    }

    static void setupTriangle_implementation(UniformData uniform, uint32 tri_idx, VertexData v1, VertexData v2,
                                             VertexData v3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // // Setup interpolation of light intensity over triangle.
        // triangle->intensity.initialize(v1.buffer->intensity, v2.buffer->intensity, v3.buffer->intensity, bc_incs, v1_offset);

        // Setup interpolation of uv coordinates over triangle.
        triangle->uv.initialize(v1.source->uv_coordinates, v2.source->uv_coordinates, v3.source->uv_coordinates, bc_incs, v1_offset);
    }
};

} // namespace MicroRenderer
