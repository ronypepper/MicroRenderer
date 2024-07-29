cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(MicroRenderer test/main.cpp
        include/MicroRenderer/MicroRenderer.h
        include/MicroRenderer/Math/Vector4.h
        include/MicroRenderer/Math/Vector3.h
        include/MicroRenderer/Math/Matrix4.h
        include/MicroRenderer/Math/ScalarTypes.h
        include/MicroRenderer/Math/Vector2.h
        include/MicroRenderer/Math/Transform.tpp
        include/MicroRenderer/Math/Transform.h
        include/MicroRenderer/Core/Renderer.tpp
        include/MicroRenderer/Core/Renderer.h
        include/MicroRenderer/Core/FrameBuffer.h
        include/MicroRenderer/Core/Framebuffer.tpp
        include/MicroRenderer/Math/Utility.h
        include/MicroRenderer/Core/Shading/VertexShader.h
        include/MicroRenderer/Core/Shading/FragmentShader.h
        include/MicroRenderer/Core/Shading/ShaderProgram.h
        include/MicroRenderer/Core/Shading/ShaderInterface.h
        include/MicroRenderer/Shaders/CustomShader/CustomVertexShader.h
        include/MicroRenderer/Shaders/CustomShader/CustomFragmentShader.h
        include/MicroRenderer/Shaders/CustomShader/CustomShaderInterface.h
        include/MicroRenderer/Core/Shading/TriangleAssembler.h
        include/MicroRenderer/Shaders/CustomShader/CustomTriangleAssembler.h
        include/MicroRenderer/Shaders/CustomShader/CustomShaderProgram.h
)

target_include_directories(MicroRenderer PUBLIC include)

# The .tpp files seem to confuse cmake's language selection.
set_target_properties(MicroRenderer PROPERTIES LINKER_LANGUAGE CXX)

