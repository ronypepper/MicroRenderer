//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "CustomFragmentShader.h"
#include "CustomTriangleAssembler.h"
#include "CustomVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using CustomShaderProgram = BaseShaderProgram<T, t_cfg, CustomVertexShader, CustomTriangleAssembler, CustomFragmentShader>;

} // namespace MicroRenderer
