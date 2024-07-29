//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

namespace MicroRenderer {

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
struct CustomTriangleSource
{
    // Define interface here.
};

template<typename T>
struct CustomTriangleBuffer
{
    // Define interface here.
};

template<typename T>
using CustomShaderInterface = BaseShaderInterface<CustomGlobalData<T>, CustomInstanceData<T>, CustomVertexSource<T>, CustomVertexBuffer<T>, CustomTriangleSource<T>, CustomTriangleBuffer<T>>;

} // namespace MicroRenderer
