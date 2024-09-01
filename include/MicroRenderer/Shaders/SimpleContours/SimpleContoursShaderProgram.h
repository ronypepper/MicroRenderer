//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "SimpleContoursFragmentShader.h"
#include "SimpleContoursTriangleAssembler.h"
#include "SimpleContoursVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using SimpleContoursShaderProgram = BaseShaderProgram<T, t_cfg, SimpleContoursVertexShader, SimpleContoursTriangleAssembler, SimpleContoursFragmentShader>;

} // namespace MicroRenderer
