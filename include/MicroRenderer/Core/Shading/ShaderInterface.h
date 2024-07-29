//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Math/Vector4.h"

namespace MicroRenderer {

template<typename T>
struct BaseOrthoVertexBuffer
{
    Vector3<T> screen_position;
};

template<typename T>
struct BasePerspVertexBuffer
{
    Vector4<T> clip_position;
    Vector3<T> screen_position;
};

struct TriangleIndices
{
    uint16 vertex_1_idx;
    uint16 vertex_2_idx;
    uint16 vertex_3_idx;
};

template<class GlobalData, class InstanceData, class VertexSource, class VertexBuffer, class TriangleSource, class TriangleBuffer>
class BaseShaderInterface
{
public:
    using GlobalData_type = GlobalData;
    using InstanceData_type = InstanceData;
    using VertexSource_type = VertexSource;
    using VertexBuffer_type = VertexBuffer;
    using TriangleSource_type = TriangleSource;
    using TriangleBuffer_type = TriangleBuffer;
    struct UniformData
    {
        const GlobalData* global;
        const InstanceData* instance;
    };
    struct VertexData
    {
        const VertexSource* source;
        VertexBuffer* buffer;
    };
    struct TriangleData
    {
        const TriangleSource* source;
        TriangleBuffer* buffer;
    };
    struct ModelData
    {
        uint16 num_vertices;
        uint16 num_triangles;
        VertexSource* vertices;
        TriangleSource* triangles;
        TriangleIndices* indices;
    };
};

#define USE_SHADER_INTERFACE_TYPES(Interface) \
    using GlobalData = typename Interface::GlobalData_type; \
    using InstanceData = typename Interface::InstanceData_type; \
    using UniformData = typename Interface::UniformData; \
    using VertexSource = typename Interface::VertexSource_type; \
    using VertexBuffer = typename Interface::VertexBuffer_type; \
    using VertexData = typename Interface::VertexData; \
    using TriangleSource = typename Interface::TriangleSource_type; \
    using TriangleBuffer = typename Interface::TriangleBuffer_type; \
    using TriangleData = typename Interface::TriangleData; \
    using ModelData = typename Interface::ModelData;

} // namespace MicroRenderer
