//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class CustomVertexShader : public BaseVertexShader<T, CustomShaderInterface, CustomVertexShader>
{
public:
    USE_SHADER_INTERFACE(CustomShaderInterface<T>);

    static void shadeVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
