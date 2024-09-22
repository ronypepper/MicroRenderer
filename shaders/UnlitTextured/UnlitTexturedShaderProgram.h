//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderProgram.h"
#include "UnlitTexturedFragmentShader.h"
#include "UnlitTexturedTriangleAssembler.h"
#include "UnlitTexturedVertexShader.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
using UnlitTexturedShaderProgram = BaseShaderProgram<T, t_cfg, UnlitTexturedVertexShader, UnlitTexturedTriangleAssembler, UnlitTexturedFragmentShader>;

} // namespace MicroRenderer
