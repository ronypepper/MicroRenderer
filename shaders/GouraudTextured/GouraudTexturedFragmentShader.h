//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "GouraudTexturedShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class GouraudTexturedFragmentShader : public BaseFragmentShader<T, t_cfg, GouraudTexturedShaderInterface, GouraudTexturedFragmentShader>
{
public:
    using ShaderInterface_type = GouraudTexturedShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        // Interpolate light intensity.
        triangle->intensity.template increment<mode>(offset);

        // Interpolate uv coordinates.
        triangle->uv.template increment<mode>(offset);
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Texture.
        auto color = uniform.instance->color_texture.readPixelAt(static_cast<Vector2<T>>(triangle->uv.getValue()));
        //const Vector3<T> intensity = triangle->intensity.getValue();
        //return {color.g * intensity, color.b * intensity, 1.f, color.r * intensity};
        return color;
    }
};

} // namespace MicroRenderer
