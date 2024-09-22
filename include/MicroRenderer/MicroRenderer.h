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

// // Shaders/CustomShader
// #include "CustomShader/CustomShaderInterface.h"
// #include "CustomShader/CustomShaderProgram.h"
// #include "CustomShader/CustomVertexShader.h"
// #include "CustomShader/CustomTriangleAssembler.h"
// #include "CustomShader/CustomFragmentShader.h"
//
// // Shaders/SimpleContours
// #include "SimpleContours/SimpleContoursShaderInterface.h"
// #include "SimpleContours/SimpleContoursShaderProgram.h"
// #include "SimpleContours/SimpleContoursVertexShader.h"
// #include "SimpleContours/SimpleContoursTriangleAssembler.h"
// #include "SimpleContours/SimpleContoursFragmentShader.h"
//
// // Shaders/UnlitTextured
// #include "UnlitTextured/UnlitTexturedShaderInterface.h"
// #include "UnlitTextured/UnlitTexturedShaderProgram.h"
// #include "UnlitTextured/UnlitTexturedVertexShader.h"
// #include "UnlitTextured/UnlitTexturedTriangleAssembler.h"
// #include "UnlitTextured/UnlitTexturedFragmentShader.h"
//
// // Shaders/GouraudTextured
// #include "GouraudTextured/GouraudTexturedShaderInterface.h"
// #include "GouraudTextured/GouraudTexturedShaderProgram.h"
// #include "GouraudTextured/GouraudTexturedVertexShader.h"
// #include "GouraudTextured/GouraudTexturedTriangleAssembler.h"
// #include "GouraudTextured/GouraudTexturedFragmentShader.h"
