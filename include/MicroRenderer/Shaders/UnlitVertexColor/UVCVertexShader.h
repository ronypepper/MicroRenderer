//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCVertexShader : public BaseVertexShader<T, UVCShaderInterface, UVCVertexShader>
{
public:
    USE_SHADER_INTERFACE(UVCShaderInterface<T>);

    static void transformVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // auto global = uniform.global;
        // auto instance = uniform.instance;
        // auto source = vertex.source;
        // auto buffer = vertex.buffer;
        //vertex.buffer->screen_position = (uniform.instance->model_screen_tf * Vector4<T>{vertex.source->model_position, static_cast<T>(1.0)}).getXYZ();
        vertex.buffer->clip_position = uniform.instance->model_screen_tf * Vector4<T>{vertex.source->model_position, static_cast<T>(1.0)};
    }

    static void shadeVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
