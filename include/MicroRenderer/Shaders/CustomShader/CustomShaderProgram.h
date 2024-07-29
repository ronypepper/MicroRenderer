//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "CustomFragmentShader.h"
#include "CustomShaderInterface.h"
#include "CustomTriangleAssembler.h"
#include "CustomVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T>
using CustomOrthoShaderProgram = BaseShaderProgram<T, ORTHOGRAPHIC, CustomShaderInterface<T>, CustomVertexShader<T>, CustomTriangleAssembler<T>, CustomFragmentShader<T>>;

template<typename T>
using CustomPerspShaderProgram = BaseShaderProgram<T, PERSPECTIVE, CustomShaderInterface<T>, CustomVertexShader<T>, CustomTriangleAssembler<T>, CustomFragmentShader<T>>;

} // namespace MicroRenderer
