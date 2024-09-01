//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class CustomFragmentShader : public BaseFragmentShader<T, t_cfg, CustomShaderInterface, CustomFragmentShader>
{
public:
    using ShaderInterface_type = CustomShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        // Implement here.
    }
    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Implement here.
        return {static_cast<T>(1.0)};
    }
};

} // namespace MicroRenderer
