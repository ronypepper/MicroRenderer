//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "UVCFragmentShader.h"
#include "UVCTriangleAssembler.h"
#include "UVCVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using UVCShaderProgram = BaseShaderProgram<T, t_cfg, UVCVertexShader, UVCTriangleAssembler, UVCFragmentShader>;

} // namespace MicroRenderer
