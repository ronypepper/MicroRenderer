//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCVertexShader : public BaseVertexShader<T, UVCShaderInterface<T>, UVCVertexShader<T>>
{
    USE_SHADER_INTERFACE_TYPES(UVCShaderInterface<T>)
public:
    static void transformVertex_implementation(const UniformData uniform, const VertexData vertex)
    {
        // auto global = uniform.global;
        // auto instance = uniform.instance;
        // auto source = vertex.source;
        // auto buffer = vertex.buffer;
        //vertex.buffer->screen_position = (uniform.instance->model_screen_tf * Vector4<T>{vertex.source->model_position, static_cast<T>(1.0)}).getXYZ();
        vertex.buffer->clip_position = uniform.instance->model_screen_tf * Vector4<T>{vertex.source->model_position, static_cast<T>(1.0)};
    }
    static const Vector4<T>& getHomogenousSpacePosition_implementation(const VertexData vertex)
    {
        return vertex.buffer->clip_position;
    }
    static const Vector3<T>& getScreenSpacePosition_implementation(const VertexData vertex)
    {
        return vertex.buffer->screen_position;
    }
    static void setHomogenousSpacePosition_implementation(VertexBuffer* const buffer, const Vector4<T>& position)
    {
        buffer->clip_position = position;
    }
    static void setScreenSpacePosition_implementation(VertexBuffer* const buffer, const Vector3<T>& position)
    {
        buffer->screen_position = position;
    }
    static void shadeVertex_implementation(const UniformData uniform, const VertexData vertex)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
