//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/FragmentShader.h"
#include "SimpleContoursShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class SimpleContoursFragmentShader : public BaseFragmentShader<T, t_cfg, SimpleContoursShaderInterface, SimpleContoursFragmentShader>
{
public:
    using ShaderInterface_type = SimpleContoursShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes_implementation(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
    }

    static ShaderOutput computeColor_implementation(UniformData uniform, TriangleBuffer* triangle)
    {
        // Return flat shading.
        //return {triangle->shading.g, triangle->shading.b, 1.f, triangle->shading.r};
        return triangle->shading;
    }
};

} // namespace MicroRenderer
