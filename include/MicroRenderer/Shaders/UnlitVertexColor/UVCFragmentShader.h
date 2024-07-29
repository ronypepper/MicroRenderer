//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCFragmentShader : public BaseFragmentShader<T, UVCShaderInterface<T>, UVCFragmentShader<T>>
{
    USE_SHADER_INTERFACE_TYPES(UVCShaderInterface<T>)
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
        return triangle.source->color;
    }
};

} // namespace MicroRenderer
