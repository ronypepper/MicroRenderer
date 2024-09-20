//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class CustomTriangleAssembler : public BaseTriangleAssembler<T, t_cfg, CustomShaderInterface, CustomTriangleAssembler>
{
public:
    using ShaderInterface_type = CustomShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void interpolateVertices_implementation(UniformData uniform, VertexData from, VertexData to,
                                                   VertexSource* new_src, VertexBuffer* new_buf, T from_factor,
                                                   T to_factor)
    {
        // Implement here.
    }
    static void setupTriangle_implementation(UniformData uniform, uint32 tri_idx, VertexData v1, VertexData v2,
                                             VertexData v3, TriangleBuffer* triangle, Vector2<T> v1_offset,
                                             const BarycentricIncrements<T>& bc_incs)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
