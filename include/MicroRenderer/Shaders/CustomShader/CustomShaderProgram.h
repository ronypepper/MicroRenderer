//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "CustomFragmentShader.h"
#include "CustomTriangleAssembler.h"
#include "CustomVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

constexpr ShaderProgramConfig costum_shader_program_config = {
    ORTHOGRAPHIC, CULL_AT_SCREEN_BORDER, CLIP_AT_NEAR_PLANE, DEPTH_TEST_ENABLED, SHADING_ENABLED
};

template<typename T>
using CustomShaderProgram = BaseShaderProgram<T, costum_shader_program_config, CustomVertexShader, CustomTriangleAssembler, CustomFragmentShader>;

} // namespace MicroRenderer
