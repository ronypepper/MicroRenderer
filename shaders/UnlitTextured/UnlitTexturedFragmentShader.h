//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "UnlitTexturedShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class UnlitTexturedFragmentShader : public BaseFragmentShader<T, t_cfg, UnlitTexturedShaderInterface, UnlitTexturedFragmentShader>
{
public:
    using ShaderInterface_type = UnlitTexturedShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        // Interpolate uv coordinates.
        triangle->uv.template increment<mode>(offset);
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Return color from texture.
        auto color = uniform.instance->color_texture.readPixelAt(static_cast<Vector2<T>>(triangle->uv.getValue()));
        //return {color.g, color.b, 1.f, color.r};
        return color;
    }
};

} // namespace MicroRenderer
