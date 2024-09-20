cmake_minimum_required(VERSION 3.28)
project(MicroRenderer)

set(CMAKE_CXX_STANDARD 20)

# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_executable(MicroRenderer test/3d_freefly_viewer.cpp)

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
