//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class UVCFragmentShader : public BaseFragmentShader<T, t_cfg, UVCShaderInterface, UVCFragmentShader>
{
public:
    using ShaderInterface_type = UVCShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Return flat shading.
        return triangle->shading;
    }
};

} // namespace MicroRenderer
