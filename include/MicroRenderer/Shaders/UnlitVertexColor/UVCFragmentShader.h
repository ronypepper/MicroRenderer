//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCFragmentShader : public BaseFragmentShader<T, UVCShaderInterface, UVCFragmentShader>
{
public:
    USE_SHADER_INTERFACE(UVCShaderInterface<T>);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        // Interpolate vertex color.
        triangle->color.increment<mode>(offset);

        // Interpolate uv coordinates.
        triangle->uv.increment<mode>(offset);
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Vertex color.
        //return (triangle->color + Vector3<T>(1.f)) * static_cast<T>(0.5);

        // Texture.
        auto color = uniform.instance->texture_color.samplePixelAt(static_cast<Vector2<T>>(triangle->uv.getValue()));
        //return {color.g, color.b, 1.f, color.r};
        return uniform.instance->texture_color.samplePixelAt(static_cast<Vector2<T>>(triangle->uv.getValue()));
    }
};

} // namespace MicroRenderer
