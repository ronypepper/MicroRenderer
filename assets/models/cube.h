//
// Created by pusdo on 13/08/2024.
//

#pragma once
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

inline Vector3<uint8> test_texture_rgb888[3 * 3] = {
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255},
    {0, 0, 255}, {255, 0, 0}, {0, 255, 0},
    {0, 255, 0}, {0, 0, 255}, {255, 0, 0}
};

inline Vector3<uint8> test_texture2_rgb888[3 * 3] = {
    {25, 0, 90}, {200, 125, 20}, {170, 0, 25},
    {30, 10, 255}, {255, 0, 100}, {100, 25, 220},
    {0, 25, 10}, {120, 0, 255}, {255, 0, 50}
};

// inline Vector3<uint8> test_texture_rgb565[3 * 3] = {
//     {31, 0, 0}, {0, 63, 0}, {0, 0, 31},
//     {0, 0, 31}, {31, 0, 0}, {0, 63, 0},
//     {0, 63, 0}, {0, 0, 31}, {31, 0, 0}
// };
//
// inline Vector3<uint8> test_texture_rgb444[3 * 3] = {
//     {15, 0, 0}, {0, 15, 0}, {0, 0, 15},
//     {0, 0, 15}, {15, 0, 0}, {0, 15, 0},
//     {0, 15, 0}, {0, 0, 15}, {15, 0, 0}
// };

inline UVCVertexSource<float> cube_vertex_source[8] = {
    {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}},
    {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.f, 1.f}},
    {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {1.f, 0.f}},
    {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {1.f, 1.f}},
    {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {0.f, 0.f}},
    {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {0.f, 1.f}},
    {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f, 0.f}},
    {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f, 1.f}}
};

inline UVCVertexSource<float> cube_vertex_source1[8] = {
    {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {-1.f, -1.f}},
    {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {-1.f, 2.f}},
    {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {2.f, -1.f}},
    {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {2.f, 2.f}},
    {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {-5.f, -1.f}},
    {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {-5.f, 2.f}},
    {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {2.f, -1.f}},
    {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {2.f, 2.f}}
};

inline UVCVertexSource<float> cube_vertex_source2[8] = {
    {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.0001f, 0.0001f}},
    {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}, {0.0001f, 1.f - 0.0001f}},
    {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}, {1.f - 0.0001f, 0.0001f}},
    {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {1.f - 0.0001f, 1.f - 0.0001f}},
    {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}, {0.0001f, 0.0001f}},
    {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}, {0.0001f, 1.f - 0.0001f}},
    {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f - 0.0001f, 0.0001f}},
    {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f - 0.0001f, 1.f - 0.0001f}}
};

inline TriangleIndices cube_indices[12] = {
    4, 2, 0, 2, 7, 3, 6, 5, 7, 1, 7, 5, 0, 3, 1, 4, 1, 5, 4, 6, 2, 2, 6, 7, 6, 4, 5, 1, 3, 7, 0, 2, 3, 4, 0, 1
};

inline UVCShaderInterface<float>::ModelData cube_model = {8, 12, cube_vertex_source1, cube_indices};

} // namespace MicroRenderer
