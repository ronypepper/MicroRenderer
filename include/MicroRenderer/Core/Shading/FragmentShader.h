//
// Created by pusdo on 29/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, class ShaderInterface, class Derived>
class BaseFragmentShader
{
    USE_SHADER_INTERFACE_TYPES(ShaderInterface)
public:
    static void interpolateTo(const UniformData uniform, const TriangleData triangle, const int32 x, const int32 y)
    {
        Derived::interpolateTo_implementation(uniform, triangle, x, y);
    }
    static void interpolateRight(const UniformData uniform, const TriangleData triangle)
    {
        Derived::interpolateRight_implementation(uniform, triangle);
    }
    static Vector3<T> shadeFragment(const UniformData uniform, const TriangleData triangle)
    {
        return Derived::shadeFragment_implementation(uniform, triangle);
    }
};

} // namespace MicroRenderer
