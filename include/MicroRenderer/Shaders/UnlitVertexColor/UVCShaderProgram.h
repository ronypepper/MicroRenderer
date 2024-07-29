//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "UVCFragmentShader.h"
#include "UVCShaderInterface.h"
#include "UVCTriangleAssembler.h"
#include "UVCVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T>
using UVCOrthoShaderProgram = BaseShaderProgram<T, ORTHOGRAPHIC, UVCShaderInterface<T>, UVCVertexShader<T>, UVCTriangleAssembler<T>, UVCFragmentShader<T>>;

template<typename T>
using UVCPerspShaderProgram = BaseShaderProgram<T, PERSPECTIVE, UVCShaderInterface<T>, UVCVertexShader<T>, UVCTriangleAssembler<T>, UVCFragmentShader<T>>;

} // namespace MicroRenderer
