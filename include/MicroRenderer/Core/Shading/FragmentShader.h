//
// Created by pusdo on 29/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, template <typename> class Interface, template <typename> class Derived>
class BaseFragmentShader
{
public:
    USE_SHADER_INTERFACE(Interface<T>);

    static void interpolateTo(UniformData uniform, Fragment* fragment, TriangleData triangle, VertexData vertex,
                              const Vector2<T>& offset, int32 x, int32 y)
    {
        Derived<T>::interpolateTo_implementation(uniform, fragment, triangle, vertex, offset, x, y);
    }

    static void interpolateRight(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        Derived<T>::interpolateRight_implementation(uniform, fragment, triangle);
    }

    static ShaderOutput computeColor(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        return Derived<T>::computeColor_implementation(uniform, fragment, triangle);
    }
};

} // namespace MicroRenderer
