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
struct UnlitTexturedGlobalData
{
};

template<typename T>
struct UnlitTexturedInstanceData : BaseInstanceData
{
    Matrix4<T> model_screen_tf;
    Texture2D<T, {ACCESS_READONLY, FORMAT_RGB444, SWIZZLE_NONE, TYPE_NORMALIZED, WRAPMODE_NONE}> color_texture;
};

template<typename T>
struct UnlitTexturedVertexSource
{
    Vector3<T> position;
    Vector2<T> uv_coordinates;
};

template<typename T, ShaderConfiguration t_cfg>
struct UnlitTexturedVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
};

template<typename T, ShaderConfiguration t_cfg>
struct UnlitTexturedTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    TriangleAttribute<T, Vector2<T>> uv;
};

template<typename T, ShaderConfiguration t_cfg>
using UnlitTexturedShaderInterface = BaseShaderInterface<T, t_cfg, UnlitTexturedGlobalData, UnlitTexturedInstanceData, UnlitTexturedVertexSource, UnlitTexturedVertexBuffer, UnlitTexturedTriangleBuffer>;

} // namespace MicroRenderer
