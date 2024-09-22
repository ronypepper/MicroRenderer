/*
* MicroRenderer model description.
 * Model name: plane
 * Target shader: MicroRenderer/Shaders/GouraudTextured
 */

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "GouraudTextured/GouraudTexturedShaderInterface.h"

using namespace MicroRenderer;

constexpr int32 plane_vertex_number = 4;

template<typename T>
constexpr GouraudTexturedVertexSource<T> plane_vertices[plane_vertex_number] = {
    {{-1.0, -1.0, -0.0}, {-0.0, -0.0, 1.0}, {1.0, 0.0}},
    {{1.0, -1.0, -0.0}, {-0.0, -0.0, 1.0}, {9.999999747378752e-05, 9.999999747378752e-05}},
    {{1.0, 1.0, 0.0}, {-0.0, -0.0, 1.0}, {0.0, 1.0}},
    {{-1.0, 1.0, 0.0}, {-0.0, -0.0, 1.0}, {0.9998999834060669, 0.9998999834060669}}
};

constexpr int32 plane_triangle_number = 2;

constexpr TriangleIndices plane_triangles[plane_triangle_number] = {
    {0, 1, 2},
    {0, 2, 3}
};

template<typename T, ShaderConfiguration t_cfg>
constexpr typename GouraudTexturedShaderInterface<T, t_cfg>::ModelData plane_model = {
    plane_vertex_number, plane_triangle_number, plane_vertices<T>, plane_triangles
};
