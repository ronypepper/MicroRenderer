//
// Created by pusdo on 18/07/2024.
//

#pragma once

// Core
#include "MicroRenderer/Core/FrameBuffer.h"
#include "MicroRenderer/Core/Renderer.h"

// Core/Shading
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Core/Shading/FragmentShader.h"

// Math
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Transform.h"
#include "MicroRenderer/Math/Utility.h"
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"

// Shaders/CustomShader
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderProgram.h"
#include "MicroRenderer/Shaders/CustomShader/CustomVertexShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomTriangleAssembler.h"
#include "MicroRenderer/Shaders/CustomShader/CustomFragmentShader.h"

// Shaders/UnlitVertexColor
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderProgram.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCVertexShader.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCTriangleAssembler.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCFragmentShader.h"