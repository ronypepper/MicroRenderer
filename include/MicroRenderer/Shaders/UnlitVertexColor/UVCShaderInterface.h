//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Vector4.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T>
struct UVCGlobalData
{
    // Define interface here.
};

template<typename T>
struct UVCInstanceData
{
    Matrix4<T> model_screen_tf;
};

template<typename T>
struct UVCVertexSource
{
    Vector3<T> model_position;
    Vector3<T> color;
};

template<typename T>
struct UVCVertexBuffer
{
    Vector4<T> clip_position;
    Vector3<T> screen_position;
};

template<typename T>
struct UVCTriangleSource
{
    // Define interface here.
    Vector3<T> color;
};

template<typename T>
struct UVCTriangleBuffer
{
    // Define interface here.
};

template<typename T>
using UVCShaderInterface = BaseShaderInterface<UVCGlobalData<T>, UVCInstanceData<T>, UVCVertexSource<T>, UVCVertexBuffer<T>, UVCTriangleSource<T>, UVCTriangleBuffer<T>>;

} // namespace MicroRenderer
