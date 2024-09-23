//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/ShaderProgram.h"
#include "SimpleContoursFragmentShader.h"
#include "SimpleContoursTriangleAssembler.h"
#include "SimpleContoursVertexShader.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using SimpleContoursShaderProgram = BaseShaderProgram<T, t_cfg, SimpleContoursVertexShader, SimpleContoursTriangleAssembler, SimpleContoursFragmentShader>;

} // namespace MicroRenderer
