//
// Created by pusdo on 22/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

namespace MicroRenderer {

template<typename T, class ShaderInterface, class Derived>
class BaseVertexShader
{
    USE_SHADER_INTERFACE_TYPES(ShaderInterface)
public:
    static void transformVertex(const UniformData uniform, const VertexData vertex)
    {
        Derived::transformVertex_implementation(uniform, vertex);
    }
    static const Vector4<T>& getHomogenousSpacePosition(const VertexData vertex)
    {
        return Derived::getHomogenousSpacePosition_implementation(vertex);
    }
    static const Vector3<T>& getScreenSpacePosition(const VertexData vertex)
    {
        return Derived::getScreenSpacePosition_implementation(vertex);
    }
    static void setHomogenousSpacePosition(VertexBuffer* const buffer, const Vector4<T>& position)
    {
        Derived::setHomogenousSpacePosition_implementation(buffer, position);
    }
    static void setScreenSpacePosition(VertexBuffer* const buffer, const Vector3<T>& position)
    {
        Derived::setScreenSpacePosition_implementation(buffer, position);
    }
    static void shadeVertex(const UniformData uniform, const VertexData vertex)
    {
        Derived::shadeVertex_implementation(uniform, vertex);
    }
};

} // namespace MicroRenderer
