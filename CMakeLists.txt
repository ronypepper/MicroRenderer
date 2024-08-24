cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 20)

#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})

#add_executable(MicroRendererImageSaver test/ppm_image_saver.cpp)
#target_link_libraries(MicroRendererImageSaver ${SDL2_LIBRARIES})

#add_executable(MicroRenderer3dViewer test/3d_freefly_viewer.cpp)
#target_link_libraries(MicroRenderer3dViewer ${SDL2_LIBRARIES})

#add_subdirectory(third-party/SDL2 EXCLUDE_FROM_ALL)

#link_directories(third-party/SDL2/lib/cmake/SDL2)

# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_executable(MicroRenderer test/3d_freefly_viewer.cpp
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
        assets/models/cube.h
        assets/models/rasterization_test.h
        include/MicroRenderer/Core/Textures/Texture2D.h
        include/MicroRenderer/Core/Textures/TextureTypes.h
        include/MicroRenderer/Core/Textures/TextureConfiguration.h
        include/MicroRenderer/Math/Interpolation.h
)

# The .tpp files seem to confuse cmake's language selection.
set_target_properties(MicroRenderer PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(MicroRenderer PUBLIC include)
target_include_directories(MicroRenderer PUBLIC assets)
target_include_directories(MicroRenderer PUBLIC third-party)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(MicroRenderer PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(MicroRenderer PRIVATE SDL2::SDL2-static)
