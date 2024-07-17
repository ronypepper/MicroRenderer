cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 17)

add_library(MicroRenderer STATIC
        include/MicroRenderer/Math/Vector4.h
        include/MicroRenderer/Math/Vector3.h
        include/MicroRenderer/Math/Matrix4.h
        include/MicroRenderer/Math/ScalarTypes.h
        include/MicroRenderer/Math/Vector2.h
        src/Math/Transform.cpp
        include/MicroRenderer/Math/Transform.h
        src/Core/Renderer.cpp
        include/MicroRenderer/Core/Renderer.h
        src/Core/FrameBuffer.cpp
        include/MicroRenderer/Core/FrameBuffer.h
)

target_include_directories(MicroRenderer PUBLIC include)
