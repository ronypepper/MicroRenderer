//
// Created by pusdo on 13/08/2024.
//

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

using namespace MicroRenderer;

constexpr int32 cube_vertex_number = 8;

// constexpr UVCVertexSource<float> cube_vertex_source[cube_vertex_number] = {
//     {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
//     {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 1.f}},
//     {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {1.f, 0.f}},
//     {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {1.f, 1.f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {0.f, 0.f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {0.f, 1.f}},
//     {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f, 0.f}},
//     {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f, 1.f}}
// };

template<typename T>
constexpr UVCVertexSource<T> cube_vertices[cube_vertex_number] = {
    {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {-1.f, -1.f}},
    {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {-1.f, 2.f}},
    {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {2.f, -1.f}},
    {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {2.f, 2.f}},
    {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {-5.f, -1.f}},
    {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {-5.f, 2.f}},
    {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {2.f, -1.f}},
    {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {2.f, 2.f}}
};

// inline UVCVertexSource<float> cube_vertex_source2[8] = {
//     {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.0001f, 0.0001f}},
//     {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.0001f, 1.f - 0.0001f}},
//     {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {1.f - 0.0001f, 0.0001f}},
//     {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {1.f - 0.0001f, 1.f - 0.0001f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {0.0001f, 0.0001f}},
//     {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {0.0001f, 1.f - 0.0001f}},
//     {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f - 0.0001f, 0.0001f}},
//     {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f - 0.0001f, 1.f - 0.0001f}}
// };

constexpr int32 cube_triangle_number = 12;

constexpr TriangleIndices cube_triangles[cube_triangle_number] = {
    4, 2, 0, 2, 7, 3, 6, 5, 7, 1, 7, 5, 0, 3, 1, 4, 1, 5, 4, 6, 2, 2, 6, 7, 6, 4, 5, 1, 3, 7, 0, 2, 3, 4, 0, 1
};

template<typename T>
constexpr typename UVCShaderInterface<T>::ModelData cube_model = {
    cube_vertex_number, cube_triangle_number, cube_vertices<T>, cube_triangles
};
