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
struct CustomInstanceData : BaseInstanceData
{
    // Define interface here.
};

template<typename T>
struct CustomVertexSource
{
    // Define interface here.
};

template<typename T, ShaderConfiguration t_cfg>
struct CustomVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
    // Define interface here.
};

template<typename T, ShaderConfiguration t_cfg>
struct CustomTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    // Define interface here.
};

template<typename T, ShaderConfiguration t_cfg>
using CustomShaderInterface = BaseShaderInterface<T, t_cfg, CustomGlobalData, CustomInstanceData, CustomVertexSource, CustomVertexBuffer, CustomTriangleBuffer>;

} // namespace MicroRenderer
