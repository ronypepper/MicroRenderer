//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/ShaderInterface.h"

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
    static void setupTriangle(UniformData uniform, uint32 tri_idx, VertexData v1, VertexData v2, VertexData v3,
                              TriangleBuffer* triangle, Vector2<T> v1_offset, const BarycentricIncrements<T>& bc_incs)
    {
        Derived<T, t_cfg>::setupTriangle_implementation(uniform, tri_idx, v1, v2, v3, triangle, v1_offset, bc_incs);
    }
};

} // namespace MicroRenderer
