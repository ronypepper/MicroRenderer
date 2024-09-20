//
// Created by pusdo on 18/07/2024.
//

#pragma once

// Core
#include "MicroRenderer/Core/Renderer.h"

// Core/Shading
#include "MicroRenderer/Core/Shading/ShaderConfiguration.h"
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Core/Shading/TriangleAssembler.h"
#include "MicroRenderer/Core/Shading/FragmentShader.h"

// Core/Textures
#include "MicroRenderer/Core/Textures/Texture2D.h"
#include "MicroRenderer/Core/Textures/TextureTypes.h"
#include "MicroRenderer/Core/Textures/TextureConfiguration.h"

// Math
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Matrix3.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Transform.h"
#include "MicroRenderer/Math/Utility.h"
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"
#include "MicroRenderer/Math/Interpolation.h"

// Shaders/CustomShader
#include "MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h"
#include "MicroRenderer/Shaders/CustomShader/CustomShaderProgram.h"
#include "MicroRenderer/Shaders/CustomShader/CustomVertexShader.h"
#include "MicroRenderer/Shaders/CustomShader/CustomTriangleAssembler.h"
#include "MicroRenderer/Shaders/CustomShader/CustomFragmentShader.h"

// Shaders/SimpleContours
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursShaderInterface.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursShaderProgram.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursVertexShader.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursTriangleAssembler.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursFragmentShader.h"

// Shaders/GouraudTextured
#include "MicroRenderer/Shaders/GouraudTextured/GouraudTexturedShaderInterface.h"
#include "MicroRenderer/Shaders/GouraudTextured/GouraudTexturedShaderProgram.h"
#include "MicroRenderer/Shaders/GouraudTextured/GouraudTexturedVertexShader.h"
#include "MicroRenderer/Shaders/GouraudTextured/GouraudTexturedTriangleAssembler.h"
#include "MicroRenderer/Shaders/GouraudTextured/GouraudTexturedFragmentShader.h"
