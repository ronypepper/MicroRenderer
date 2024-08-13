//
// Created by pusdo on 03/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

enum ProjectionType
{
    ORTHOGRAPHIC,
    PERSPECTIVE
};

template <typename T, ProjectionType Projection, class ShaderInterface, class VertexShader, class TriangleAssembler, class FragmentShader>
class BaseShaderProgram
{
    //static_assert(std::is_base_of_v<BaseVertexShader, VertexShader>, "TriangleSource must inherit from BaseTriangleSource!");
    //static_assert(std::is_base_of_v<BaseTriangleSource, TriangleSource>, "TriangleSource must inherit from BaseTriangleSource!");
    //static_assert(std::is_base_of_v<BaseTriangleSource, TriangleSource>, "TriangleSource must inherit from BaseTriangleSource!");
public:
    using ShaderInterface_type = ShaderInterface;
    using VertexShader_type = VertexShader;
    using TriangleAssembler_type = TriangleAssembler;
    using FragmentShader_type = FragmentShader;
    USE_SHADER_INTERFACE_TYPES(ShaderInterface)

    static constexpr ProjectionType getProjectionType()
    {
        return Projection;
    }
    void setGlobalData(const GlobalData* data)
    {
        uniform_data.global = data;
    }
    void setInstanceData(const InstanceData* data)
    {
        uniform_data.instance = data;
    }
    void setNearPlane(T distance)
    {
        near_plane = distance;
        inv_near_plane = static_cast<T>(1.0) / distance;
    }
    T getNearPlane() const
    {
        return near_plane;
    }

    void transformVertex(const VertexData vertex)
    {
        VertexShader::transformVertex(uniform_data, vertex);
    }
    const Vector4<T>& getHomogenousSpacePosition(const VertexData vertex)
    {
        return VertexShader::getHomogenousSpacePosition(vertex);
    }
    const Vector3<T>& getScreenSpacePosition(const VertexData vertex)
    {
        return VertexShader::getScreenSpacePosition(vertex);
    }
    void setHomogenousSpacePosition(VertexBuffer* const buffer, const Vector4<T>& position)
    {
        VertexShader::setHomogenousSpacePosition(buffer, position);
    }
    void setScreenSpacePosition(VertexBuffer* const buffer, const Vector3<T>& position)
    {
        VertexShader::setScreenSpacePosition(buffer, position);
    }
    void shadeVertex(const VertexData vertex)
    {
        VertexShader::shadeVertex(uniform_data, vertex);
    }

    void interpolateVertices(const VertexData from, const VertexData to, VertexSource* new_src, VertexBuffer* new_buf)
    {
        if constexpr (Projection == ORTHOGRAPHIC) {
            const Vector3<T>& from_clip_pos = getScreenSpacePosition(from);
            const Vector3<T>& to_clip_pos = getScreenSpacePosition(to);
            const T alpha = from_clip_pos.z / (from_clip_pos.z - to_clip_pos.z);
            const T beta = static_cast<T>(1.0) - alpha;
            const T new_clip_x = from_clip_pos.x * beta + to_clip_pos.x * alpha;
            const T new_clip_y = from_clip_pos.y * beta + to_clip_pos.y * alpha;
            setScreenSpacePosition(new_buf, {new_clip_x, new_clip_y, static_cast<T>(0.0)});
            TriangleAssembler::interpolateVertices(uniform_data, from, to, new_src, new_buf, alpha);
        }
        else if constexpr (Projection == PERSPECTIVE) {
            const Vector4<T>& from_clip_pos = getHomogenousSpacePosition(from);
            const Vector4<T>& to_clip_pos = getHomogenousSpacePosition(to);
            const T alpha = from_clip_pos.z / (from_clip_pos.z - to_clip_pos.z);
            const T beta = static_cast<T>(1.0) - alpha;
            const T new_clip_x = from_clip_pos.x * beta + to_clip_pos.x * alpha;
            const T new_clip_y = from_clip_pos.y * beta + to_clip_pos.y * alpha;
            setHomogenousSpacePosition(new_buf, {new_clip_x, new_clip_y, static_cast<T>(0.0), inv_near_plane});
            setScreenSpacePosition(new_buf, {new_clip_x * inv_near_plane, new_clip_y * inv_near_plane, static_cast<T>(0.0)});
            TriangleAssembler::interpolateVertices(uniform_data, from, to, new_src, new_buf, alpha);
        }
    }
    void setupTriangle(const VertexData vertex_1, const VertexData vertex_2, const VertexData vertex_3,
                       const TriangleData triangle)
    {
        TriangleAssembler::setupTriangle(uniform_data, vertex_1, vertex_2, vertex_3, triangle);
    }

    void interpolateTo(const TriangleData triangle, const int32 x, const int32 y)
    {
        FragmentShader::interpolateTo(uniform_data, triangle, x, y);
    }
    void interpolateRight(const TriangleData triangle)
    {
        FragmentShader::interpolateRight(uniform_data, triangle);
    }
    Vector3<T> shadeFragment(const TriangleData triangle)
    {
        return FragmentShader::shadeFragment(uniform_data, triangle);
    }
private:
    UniformData uniform_data;

    T near_plane = static_cast<T>(0.0);
    T inv_near_plane = static_cast<T>(0.0);
};

} // namespace MicroRenderer
