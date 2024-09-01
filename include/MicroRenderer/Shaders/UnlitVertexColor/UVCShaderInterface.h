//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Core/Textures/Texture2D.h"

namespace MicroRenderer {

template<typename T>
struct UVCGlobalData
{
};

template<typename T>
struct UVCInstanceData
{
    Matrix4<T> model_screen_tf;
    Vector3<T> towards_sun_dir_model_space;
    Vector3<T> color;
};

template<typename T>
struct UVCVertexSource
{
    Vector3<T> model_position;
};

template<typename T, ShaderConfiguration t_cfg>
struct UVCVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
};

template<typename T, ShaderConfiguration t_cfg>
struct UVCTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    Vector3<T> shading;
};

template<typename T, ShaderConfiguration t_cfg>
using UVCShaderInterface = BaseShaderInterface<T, t_cfg, UVCGlobalData, UVCInstanceData, UVCVertexSource, UVCVertexBuffer, UVCTriangleBuffer>;

} // namespace MicroRenderer
