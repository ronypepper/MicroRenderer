//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class CustomTriangleAssembler : public BaseTriangleAssembler<T, CustomShaderInterface, CustomTriangleAssembler>
{
public:
    USE_SHADER_INTERFACE(CustomShaderInterface<T>);

    static void interpolateVertices_implementation(UniformData uniform, VertexData from, VertexData to,
                                                   VertexSource* new_src, VertexBuffer* new_buf, T from_factor,
                                                   T to_factor)
    {
        // Implement here.
    }
    static void setupTriangle_implementation(UniformData uniform, VertexData vertex_1, VertexData vertex_2,
                                             VertexData vertex_3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
