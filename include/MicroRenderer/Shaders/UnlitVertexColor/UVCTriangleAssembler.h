//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "UVCShaderInterface.h"

namespace MicroRenderer {

    template<typename T>
    class UVCTriangleAssembler : public BaseTriangleAssembler<T, UVCShaderInterface<T>, UVCTriangleAssembler<T>>
    {
        USE_SHADER_INTERFACE_TYPES(UVCShaderInterface<T>)
    public:
        static void interpolateVertices_implementation(const UniformData uniform, const VertexData from,
                                                       const VertexData to, VertexSource* new_src,
                                                       VertexBuffer* new_buf, const T alpha)
        {
            // Implement here.
        }
        static void setupTriangle_implementation(const UniformData uniform, const VertexData vertex_1,
                                                 const VertexData vertex_2, const VertexData vertex_3,
                                                 const TriangleData triangle)
        {
            // Implement here.
        }
    };

} // namespace MicroRenderer
