//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Matrix4.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, typename T_ShaderProgram>
void preprocessTriangleNormals(const typename T_ShaderProgram::ModelData* model, Vector3<T>* normals)
{
    for (uint32 tri_idx = 0; tri_idx < model->num_triangles; ++tri_idx) {
        TriangleIndices indices = model->indices[tri_idx];
        const typename T_ShaderProgram::VertexSource& v1 = model->vertices[indices.vertex_1_idx];
        const typename T_ShaderProgram::VertexSource& v2 = model->vertices[indices.vertex_2_idx];
        const typename T_ShaderProgram::VertexSource& v3 = model->vertices[indices.vertex_3_idx];
        Vector3<T> d1 = (v2.position - v1.position).getNormalized();
        Vector3<T> d2 = (v3.position - v1.position).getNormalized();
        normals[tri_idx] = d1.cross(d2).getNormalized();
    }
}

template<typename T>
struct SimpleContoursGlobalData
{
};

template<typename T>
struct SimpleContoursInstanceData : BaseInstanceData
{
    Matrix4<T> model_screen_tf;
    const Vector3<T>* triangle_normals;
    Vector3<T> towards_sun_dir_model_space;
    Vector3<T> color;
};

template<typename T>
struct SimpleContoursVertexSource
{
    Vector3<T> position;
};

template<typename T, ShaderConfiguration t_cfg>
struct SimpleContoursVertexBuffer : BaseVertexBuffer<T, t_cfg>
{
};

template<typename T, ShaderConfiguration t_cfg>
struct SimpleContoursTriangleBuffer : BaseTriangleBuffer<T, t_cfg>
{
    Vector3<T> shading;
};

template<typename T, ShaderConfiguration t_cfg>
using SimpleContoursShaderInterface = BaseShaderInterface<T, t_cfg, SimpleContoursGlobalData,
                                                          SimpleContoursInstanceData,
                                                          SimpleContoursVertexSource, SimpleContoursVertexBuffer,
                                                          SimpleContoursTriangleBuffer>;

} // namespace MicroRenderer
