/*
 * MicroRenderer model description.
 * Model name: plane
 * Target shader: UnlitTextured
 */

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "UnlitTextured/UnlitTexturedShaderInterface.h"

using namespace MicroRenderer;

constexpr int32 plane_vertex_number = 4;

template<typename T>
constexpr UnlitTexturedVertexSource<T> plane_vertices[plane_vertex_number] = {
    {{1.0, 1.0, -0.0}, {0.9939650297164917, 0.0060350000858306885}},
    {{-1.0, 1.0, -0.0}, {0.006134000141173601, 0.006134000141173601}},
    {{-1.0, -1.0, 0.0}, {0.0060350000858306885, 0.9939650297164917}},
    {{1.0, -1.0, 0.0}, {0.9938660264015198, 0.9938660264015198}}
};

constexpr int32 plane_triangle_number = 2;

constexpr TriangleIndices plane_triangles[plane_triangle_number] = {
    {0, 1, 2},
    {0, 2, 3}
};

template<typename T, ShaderConfiguration t_cfg>
constexpr typename UnlitTexturedShaderInterface<T, t_cfg>::ModelData plane_model = {
    plane_vertex_number, plane_triangle_number, plane_vertices<T>, plane_triangles
};
