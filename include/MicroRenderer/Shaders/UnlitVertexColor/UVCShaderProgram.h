//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "UVCFragmentShader.h"
#include "UVCTriangleAssembler.h"
#include "UVCVertexShader.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

constexpr ShaderProgramConfig uvc_shader_program_config = {
    PERSPECTIVE, CULL_AT_SCREEN_BORDER, CLIP_AT_NEAR_PLANE, DEPTH_TEST_ENABLED, SHADING_ENABLED
};

template<typename T>
using UVCShaderProgram = BaseShaderProgram<T, uvc_shader_program_config, UVCVertexShader, UVCTriangleAssembler, UVCFragmentShader>;

} // namespace MicroRenderer
