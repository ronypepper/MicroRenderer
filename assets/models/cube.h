/*
 * MicroRenderer model description.
 * Model name: cube
 * Target shader: MicroRenderer/Shaders/UnlitVertexColor
 */

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

using namespace MicroRenderer;

constexpr int32 cube_vertex_number = 36;

template<typename T>
constexpr UVCVertexSource<T> cube_vertices[cube_vertex_number] = {
    {{-1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.9998999834060669, 0.9998999238014221}},
	{{1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {9.995669825002551e-05, 0.00010004099021898583}},
	{{1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.9998999238014221, 9.999290341511369e-05}},
	{{1.0, -1.0, 1.0}, {0.0, -1.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{-1.0, -1.0, -1.0}, {0.0, -1.0, 0.0}, {9.99369949568063e-05, 9.999659960158169e-05}},
	{{1.0, -1.0, -1.0}, {0.0, -1.0, 0.0}, {0.9998999238014221, 9.999659960158169e-05}},
	{{-1.0, -1.0, 1.0}, {-1.0, 0.0, 0.0}, {9.999659960158169e-05, 9.999659960158169e-05}},
	{{-1.0, 1.0, -1.0}, {-1.0, 0.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{-1.0, -1.0, -1.0}, {-1.0, 0.0, 0.0}, {0.00010008599201682955, 0.9999001026153564}},
	{{1.0, 1.0, -1.0}, {0.0, 0.0, -1.0}, {9.995669825002551e-05, 9.995669825002551e-05}},
	{{-1.0, -1.0, -1.0}, {0.0, 0.0, -1.0}, {0.9998999834060669, 0.9998999834060669}},
	{{-1.0, 1.0, -1.0}, {0.0, 0.0, -1.0}, {9.999290341511369e-05, 0.9999001026153564}},
	{{1.0, 1.0, 1.0}, {1.0, 0.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}, {9.99369949568063e-05, 9.999659960158169e-05}},
	{{1.0, 1.0, -1.0}, {1.0, 0.0, 0.0}, {0.9998999238014221, 9.999659960158169e-05}},
	{{-1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, {9.999659960158169e-05, 9.999659960158169e-05}},
	{{1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}, {0.00010008599201682955, 0.9999001026153564}},
	{{-1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.9998999834060669, 0.9998999238014221}},
	{{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {0.00010007999662775546, 0.9999001026153564}},
	{{1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {9.995669825002551e-05, 0.00010004099021898583}},
	{{1.0, -1.0, 1.0}, {0.0, -1.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{-1.0, -1.0, 1.0}, {0.0, -1.0, 0.0}, {9.999659960158169e-05, 0.9999001026153564}},
	{{-1.0, -1.0, -1.0}, {0.0, -1.0, 0.0}, {9.99369949568063e-05, 9.999659960158169e-05}},
	{{-1.0, -1.0, 1.0}, {-1.0, 0.0, 0.0}, {9.999659960158169e-05, 9.999659960158169e-05}},
	{{-1.0, 1.0, 1.0}, {-1.0, 0.0, 0.0}, {0.9999001026153564, 9.999659960158169e-05}},
	{{-1.0, 1.0, -1.0}, {-1.0, 0.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{1.0, 1.0, -1.0}, {0.0, 0.0, -1.0}, {9.995669825002551e-05, 9.995669825002551e-05}},
	{{1.0, -1.0, -1.0}, {0.0, 0.0, -1.0}, {0.9999001026153564, 9.999600297305733e-05}},
	{{-1.0, -1.0, -1.0}, {0.0, 0.0, -1.0}, {0.9998999834060669, 0.9998999834060669}},
	{{1.0, 1.0, 1.0}, {1.0, 0.0, 0.0}, {0.9999001026153564, 0.9999001026153564}},
	{{1.0, -1.0, 1.0}, {1.0, 0.0, 0.0}, {9.999659960158169e-05, 0.9999001026153564}},
	{{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}, {9.99369949568063e-05, 9.999659960158169e-05}},
	{{-1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, {9.999659960158169e-05, 9.999659960158169e-05}},
	{{1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, {0.9999001026153564, 9.999659960158169e-05}},
	{{1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}, {0.9999001026153564, 0.9999001026153564}}
};

constexpr int32 cube_triangle_number = 12;

constexpr TriangleIndices cube_triangles[cube_triangle_number] = {
    {0, 1, 2},
	{3, 4, 5},
	{6, 7, 8},
	{9, 10, 11},
	{12, 13, 14},
	{15, 16, 17},
	{18, 19, 20},
	{21, 22, 23},
	{24, 25, 26},
	{27, 28, 29},
	{30, 31, 32},
	{33, 34, 35}
};

template<typename T>
constexpr typename UVCShaderInterface<T>::ModelData cube_model = {
    cube_vertex_number, cube_triangle_number, cube_vertices<T>, cube_triangles
};
