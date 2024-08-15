//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

template<typename T>
struct BarycentricIncrements;

namespace MicroRenderer {

template<typename T, template <typename> class Interface, template <typename> class Derived>
class BaseTriangleAssembler
{
public:
    USE_SHADER_INTERFACE(Interface<T>);

    static void interpolateVertices(UniformData uniform, VertexData from, VertexData to, VertexSource* new_src,
                                    VertexBuffer* new_buf, T from_factor, T to_factor)
    {
        Derived<T>::interpolateVertices_implementation(uniform, from, to, new_src, new_buf, from_factor, to_factor);
    }
    static void setupTriangle(UniformData uniform, VertexData vertex_1, VertexData vertex_2, VertexData vertex_3,
                              TriangleData triangle, const BarycentricIncrements<T>& bc_incs)
    {
        Derived<T>::setupTriangle_implementation(uniform, vertex_1, vertex_2, vertex_3, triangle, bc_incs);
    }
};

} // namespace MicroRenderer
