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
        incrementAttributes<mode>(triangle->color, triangle->color_incs, offset);

        // Interpolate uv coordinates.
        incrementAttributes<mode>(triangle->uv, triangle->uv_incs, offset);
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Vertex color.
        //return triangle->color;

        // Texture.
        auto color = uniform.instance->texture_color.samplePixelAt(triangle->uv);
        return {color.g, color.b, 1.f, color.r};
        //return uniform.instance->texture_color.samplePixelAt(triangle->uv);
    }
};

} // namespace MicroRenderer
