cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 17)

add_library(MicroRenderer STATIC
        include/MicroRenderer/Math/Vector4.h
        include/MicroRenderer/Math/Vector3.h
        include/MicroRenderer/Math/Matrix4.h
        include/MicroRenderer/Math/ScalarTypes.h
        include/MicroRenderer/Math/Vector2.h
        include/MicroRenderer/Math/Transform.tpp
        include/MicroRenderer/Math/Transform.h
        include/MicroRenderer/Core/Renderer.tpp
        include/MicroRenderer/Core/Renderer.h
        include/MicroRenderer/Core/Framebuffer.tpp
        include/MicroRenderer/Core/FrameBuffer.h
)

target_include_directories(MicroRenderer PUBLIC include)

# The .tpp files seem to confuse cmake's language selection.
set_target_properties(MicroRenderer PROPERTIES LINKER_LANGUAGE CXX)
