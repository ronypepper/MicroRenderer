//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class CustomFragmentShader : public BaseFragmentShader<T, CustomShaderInterface<T>, CustomFragmentShader<T>>
{
    USE_SHADER_INTERFACE_TYPES(CustomShaderInterface<T>)
public:
    static void interpolateTo(const UniformData uniform, const TriangleData triangle, const int32 x, const int32 y)
    {
        // Implement here.
    }
    static void interpolateRight(const UniformData uniform, const TriangleData triangle)
    {
        // Implement here.
    }
    static Vector3<T> shadeFragment(const UniformData uniform, const TriangleData triangle)
    {
        // Implement here.
        return {static_cast<T>(1.0)};
    }
};

} // namespace MicroRenderer
