//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

namespace MicroRenderer {

    template<typename T, class ShaderInterface, class Derived>
    class BaseTriangleAssembler
    {
        USE_SHADER_INTERFACE_TYPES(ShaderInterface)
    public:
        static void interpolateVertices(const UniformData uniform, const VertexData from, const VertexData to,
                                        VertexSource* new_src, VertexBuffer* new_buf, const T alpha)
        {
            Derived::interpolateVertices_implementation(uniform, from, to, new_src, new_buf, alpha);
        }
        static void setupTriangle(const UniformData uniform, const VertexData vertex_1, const VertexData vertex_2,
                                  const VertexData vertex_3, const TriangleData triangle)
        {
            Derived::setupTriangle_implementation(uniform, vertex_1, vertex_2, vertex_3, triangle);
        }
    };

} // namespace MicroRenderer
