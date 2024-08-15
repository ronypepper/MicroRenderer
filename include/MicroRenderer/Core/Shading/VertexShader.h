//
// Created by pusdo on 22/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

namespace MicroRenderer {

template<typename T, template <typename> class Interface, template <typename> class Derived>
class BaseVertexShader
{
public:
    USE_SHADER_INTERFACE(Interface<T>);

    static void transformVertex(UniformData uniform, VertexData vertex)
    {
        Derived<T>::transformVertex_implementation(uniform, vertex);
    }
    static void shadeVertex(UniformData uniform, VertexData vertex)
    {
        Derived<T>::shadeVertex_implementation(uniform, vertex);
    }
};

} // namespace MicroRenderer
