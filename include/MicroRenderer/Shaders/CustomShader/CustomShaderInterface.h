//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

namespace MicroRenderer {

constexpr ShaderOutput custom_shader_output = {FORMAT_RGB888, TYPE_NORMALIZED};

template<typename T>
struct CustomGlobalData
{
    // Define interface here.
};

template<typename T>
struct CustomInstanceData
{
    // Define interface here.
};

template<typename T>
struct CustomVertexSource
{
    // Define interface here.
};

template<typename T>
struct CustomVertexBuffer
{
    // Define interface here.
};

template<typename T>
struct CustomTriangleBuffer
{
    // Define interface here.
};

template<typename T>
using CustomShaderInterface = BaseShaderInterface<T, custom_shader_output, CustomGlobalData, CustomInstanceData, CustomVertexSource, CustomVertexBuffer, CustomTriangleBuffer>;

} // namespace MicroRenderer
