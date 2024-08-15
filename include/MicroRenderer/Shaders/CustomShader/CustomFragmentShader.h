//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class CustomFragmentShader : public BaseFragmentShader<T, CustomShaderInterface, CustomFragmentShader>
{
public:
    USE_SHADER_INTERFACE(CustomShaderInterface<T>);

    static void interpolateTo_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle,
                                             VertexData vertex, const Vector2<T>& offset, int32 x, int32 y)
    {
        // Implement here.
    }
    static void interpolateRight_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        // Implement here.
    }
    static ShaderOutput computeColor_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        // Implement here.
        return {static_cast<T>(1.0)};
    }
};

} // namespace MicroRenderer
