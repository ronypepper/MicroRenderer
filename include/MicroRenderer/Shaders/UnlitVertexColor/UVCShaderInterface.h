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

constexpr ShaderOutput uvc_shader_output = {FORMAT_RGB888, TYPE_NORMALIZED};

template<typename T>
struct UVCGlobalData
{
};

template<typename T>
struct UVCInstanceData
{
    Matrix4<T> model_screen_tf;
    Texture2D<T, {ACCESS_READONLY, FORMAT_RGB888, SWIZZLE_NONE, TYPE_NORMALIZED, WRAPMODE_TILING}> texture_color;
};

template<typename T>
struct UVCVertexSource
{
    Vector3<T> model_position;
    Vector3<T> color;
    Vector2<T> uv_coordinates;
};

template<typename T>
struct UVCVertexBuffer : BasePerspectiveVertexBuffer<T>
{
};

template<typename T>
struct UVCTriangleBuffer : BaseDepthTriangleBuffer<T>
{
    Vector3<T> color;
    Vector2<T> uv;
    AttributeIncrements<Vector3<T>> color_incs;
    AttributeIncrements<Vector2<T>> uv_incs;
};

template<typename T>
using UVCShaderInterface = BaseShaderInterface<T, uvc_shader_output, UVCGlobalData, UVCInstanceData, UVCVertexSource, UVCVertexBuffer, UVCTriangleBuffer>;

} // namespace MicroRenderer
