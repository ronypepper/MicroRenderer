//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T>
struct SimpleContoursGlobalData
{
};

template<typename T>
struct SimpleContoursInstanceData
{
    Matrix4<T> model_screen_tf;
    Vector3<T> towards_sun_dir_model_space;
    Vector3<T> color;
};

template<typename T>
struct SimpleContoursVertexSource
{
    Vector3<T> model_position;
};

template<typename T, ShaderConfiguration t_cfg>
struct SimpleContoursVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
};

template<typename T, ShaderConfiguration t_cfg>
struct SimpleContoursTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    Vector3<T> shading;
};

template<typename T, ShaderConfiguration t_cfg>
using SimpleContoursShaderInterface = BaseShaderInterface<T, t_cfg, SimpleContoursGlobalData,
                                                          SimpleContoursInstanceData,
                                                          SimpleContoursVertexSource, SimpleContoursVertexBuffer,
                                                          SimpleContoursTriangleBuffer>;

} // namespace MicroRenderer
