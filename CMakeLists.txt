cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 17)

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
)

target_include_directories(MicroRenderer PUBLIC include)

# The .tpp files seem to confuse cmake's language selection.
set_target_properties(MicroRenderer PROPERTIES LINKER_LANGUAGE CXX)

