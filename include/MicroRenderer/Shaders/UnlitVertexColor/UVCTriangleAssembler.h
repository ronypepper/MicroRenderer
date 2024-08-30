//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCTriangleAssembler : public BaseTriangleAssembler<T, UVCShaderInterface, UVCTriangleAssembler>
{
public:
    USE_SHADER_INTERFACE(UVCShaderInterface<T>);

    static void interpolateVertices_implementation(UniformData uniform, VertexData from, VertexData to,
                                                   VertexSource* new_src, VertexBuffer* new_buf, T from_factor,
                                                   T to_factor)
    {
        new_src->color = interpolateLinearly(from.source->color, to.source->color, from_factor, to_factor);
        new_src->uv_coordinates = interpolateLinearly(from.source->uv_coordinates, to.source->uv_coordinates, from_factor, to_factor);
    }

    static void setupTriangle_implementation(UniformData uniform, VertexData vertex_1, VertexData vertex_2,
                                             VertexData vertex_3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // Setup interpolation of vertex color over triangle.
        triangle->color.initialize(vertex_1.source->color, vertex_2.source->color, vertex_3.source->color, bc_incs,
                                   v1_offset);

        // Setup interpolation of uv coordinates over triangle.
        triangle->uv.initialize(vertex_1.source->uv_coordinates, vertex_2.source->uv_coordinates,
                                vertex_3.source->uv_coordinates, bc_incs, v1_offset);
    }
};

} // namespace MicroRenderer
