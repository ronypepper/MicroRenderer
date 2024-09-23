//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Shading/ShaderProgram.h"
#include "GouraudTexturedFragmentShader.h"
#include "GouraudTexturedTriangleAssembler.h"
#include "GouraudTexturedVertexShader.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using GouraudTexturedShaderProgram = BaseShaderProgram<T, t_cfg, GouraudTexturedVertexShader, GouraudTexturedTriangleAssembler, GouraudTexturedFragmentShader>;

} // namespace MicroRenderer
