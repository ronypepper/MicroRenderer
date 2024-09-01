//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class CustomVertexShader : public BaseVertexShader<T, t_cfg, CustomShaderInterface, CustomVertexShader>
{
public:
    using ShaderInterface_type = CustomShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void shadeVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
