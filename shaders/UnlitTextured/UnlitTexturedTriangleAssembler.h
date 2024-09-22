//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "UnlitTexturedShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class UnlitTexturedTriangleAssembler : public BaseTriangleAssembler<T, t_cfg, UnlitTexturedShaderInterface, UnlitTexturedTriangleAssembler>
{
public:
    using ShaderInterface_type = UnlitTexturedShaderInterface<T, t_cfg>;
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
        // Setup interpolation of uv coordinates over triangle.
        triangle->uv.initialize(v1.source->uv_coordinates, v2.source->uv_coordinates, v3.source->uv_coordinates, bc_incs, v1_offset);
    }
};

} // namespace MicroRenderer
