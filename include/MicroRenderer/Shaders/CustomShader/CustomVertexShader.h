//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "CustomShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class CustomVertexShader : public BaseVertexShader<T, CustomShaderInterface<T>, CustomVertexShader<T>>
{
    USE_SHADER_INTERFACE_TYPES(CustomShaderInterface<T>)
public:
    static void transformVertex_implementation(const UniformData uniform, const VertexData vertex)
    {
        // Implement here.
    }
    static const Vector4<T>& getHomogenousSpacePosition_implementation(const VertexData vertex)
    {
        // Required (and only used) by PERSPECTIVE projection.
        // Implement here.
        static const Vector4<T> temp;
        return temp;
    }
    static const Vector3<T>& getScreenSpacePosition_implementation(const VertexData vertex)
    {
        // Required by ORTHOGRAPHIC and PERSPECTIVE projection.
        // Implement here.
        static const Vector3<T> temp;
        return temp;
    }
    static void setHomogenousSpacePosition_implementation(VertexBuffer* const buffer, const Vector4<T>& position)
    {
        // May be implemented (only) with PERSPECTIVE projection if interpolated homogenous coordinates are desired.
        // Implement here.
    }
    static void setScreenSpacePosition_implementation(VertexBuffer* const buffer, const Vector3<T>& position)
    {
        // Required by ORTHOGRAPHIC and PERSPECTIVE projection.
        // Implement here.
    }
    static void shadeVertex_implementation(const UniformData uniform, const VertexData vertex)
    {
        // Implement here.
    }
};

} // namespace MicroRenderer
