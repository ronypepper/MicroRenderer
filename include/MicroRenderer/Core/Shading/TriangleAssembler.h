//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

template<typename T>
struct BarycentricIncrements;

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg, template <typename, ShaderConfiguration> class Interface,
         template <typename, ShaderConfiguration> class Derived>
class BaseTriangleAssembler
{
public:
    using ShaderInterface_type = Interface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void interpolateVertices(UniformData uniform, VertexData from, VertexData to, VertexSource* new_src,
                                    VertexBuffer* new_buf, T from_factor, T to_factor)
    {
        Derived<T, t_cfg>::interpolateVertices_implementation(uniform, from, to, new_src, new_buf, from_factor, to_factor);
    }
    static void setupTriangle(UniformData uniform, VertexData vertex_1, VertexData vertex_2, VertexData vertex_3,
                              TriangleBuffer* triangle, Vector2<T> v1_offset, const BarycentricIncrements<T>& bc_incs)
    {
        Derived<T, t_cfg>::setupTriangle_implementation(uniform, vertex_1, vertex_2, vertex_3, triangle, v1_offset, bc_incs);
    }
};

} // namespace MicroRenderer
