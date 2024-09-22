//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Core/Textures/Texture2D.h"
#include "MicroRenderer/Math/Interpolation.h"

namespace MicroRenderer {

template<typename T>
struct PointLight
{
    Vector3<T> position;
    Vector3<T> intensity;
};

template<typename T>
struct GouraudTexturedGlobalData
{
    Vector3<T> ambient_intensity;
};

template<typename T>
struct GouraudTexturedInstanceData : BaseInstanceData
{
    Matrix4<T> model_screen_tf;
    Texture2D<T, {ACCESS_READONLY, FORMAT_RGB888, SWIZZLE_NONE, TYPE_INTEGER, WRAPMODE_TILING}> color_texture;
    PointLight<T> point_lights[3];
};

template<typename T>
struct GouraudTexturedVertexSource
{
    Vector3<T> position;
    Vector3<T> normal;
    Vector2<T> uv_coordinates;
};

template<typename T, ShaderConfiguration t_cfg>
struct GouraudTexturedVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
    Vector3<T> intensity;
};

template<typename T, ShaderConfiguration t_cfg>
struct GouraudTexturedTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    TriangleAttribute<T, Vector3<T>> intensity;
    TriangleAttribute<T, Vector2<T>> uv;
};

template<typename T, ShaderConfiguration t_cfg>
using GouraudTexturedShaderInterface = BaseShaderInterface<T, t_cfg, GouraudTexturedGlobalData, GouraudTexturedInstanceData, GouraudTexturedVertexSource, GouraudTexturedVertexBuffer, GouraudTexturedTriangleBuffer>;

} // namespace MicroRenderer
