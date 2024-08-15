//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include <variant>
#include "MicroRenderer/Core/Textures/Texture2D.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T>
struct RasterizationData
{
    bool last_is_left;
    uint16 instance_idx;
    //uint16 triangle_idx;
    int16 y_halftri_end;
    int16 y_fulltri_end;
    T left_dx_per_dy;
    T right_dx_per_dy;
    T last_dx_per_dy;
    T left_x;
    T right_x;
    T last_x;
};

struct RasterizationOrder
{
    int16 scanline_y;
    uint16 rasterization_idx;
};

enum FrontFaceMode
{
    COUNTERCLOCKWISE,
    CLOCKWISE
};

template<typename T, typename TriangleBuffer>
struct ScanlineRenderData
{
    uint16 num_rasterization_candidates = 0;

    RasterizationData<T>* rasterization_data_buffer = nullptr;

    VertexSource* vertex

    VertexBuffer* vertex_buffers = nullptr;

    TriangleBuffer* triangle_buffers = nullptr;

    int16 num_scanlines_per_bucket = 1;

    RasterizationOrder* rasterization_order_buffer = nullptr;

    uint16 order_buffer_actives_begin = 0;

    uint16 order_buffer_actives_end = 0;
};
    
template<typename T, template <typename> class ShaderProgram>
class Renderer {
    USE_SHADER_INTERFACE(ShaderProgram<T>::ShaderInterface);
    static constexpr TextureConfig framebuffer_config = {
        ACCESS_READWRITE, ShaderInterface::shader_output.format, SWIZZLE_NONE, ShaderInterface::shader_output.type,
        WRAPMODE_NONE
    };
    static constexpr TextureConfig depthbuffer_config = {
        ACCESS_READWRITE, FORMAT_DEPTH, SWIZZLE_NONE, TYPE_DECIMAL, WRAPMODE_NONE
    };
public:
    static constexpr ShaderProgramConfig shader_config = ShaderProgram<T>::configuration;
    using Framebuffer = std::conditional_t<shader_config.shading == SHADING_ENABLED, Texture2D<T, framebuffer_config>, std::monostate>;
    using Depthbuffer = std::conditional_t<shader_config.depth_test == DEPTH_TEST_ENABLED, Texture2D<T, depthbuffer_config>, std::monostate>;
    using NearPlaneType = std::conditional_t<shader_config.projection == PERSPECTIVE, T, std::monostate>;

    Renderer() = default;

    void setFramebuffer(void* address, uint32 width, uint32 height) requires(shader_config.shading == SHADING_ENABLED);

    Framebuffer& getFramebuffer() requires(shader_config.shading == SHADING_ENABLED);

    void setDepthbuffer(void* address, uint32 width, uint32 height) requires(shader_config.depth_test == DEPTH_TEST_ENABLED);

    Depthbuffer& getDepthbuffer() requires(shader_config.depth_test == DEPTH_TEST_ENABLED);

    void setFrontFace(FrontFaceMode front_face);

    void setNearPlane(T distance) requires(shader_config.projection == PERSPECTIVE);

    ShaderProgram<T>& getShaderProgram();

    void setRenderInstances(uint16 number, const ModelData* const* models, const InstanceData* const* instances);

    void setShadingBuffers(VertexBuffer* vertex, TriangleBuffer* triangle);

    void setRasterizationBuffers(RasterizationData<T>* data, RasterizationOrder* order);

    void rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c);
    
    void renderFramebuffer();

    void renderScanlines();

    void renderScanbuckets(int16 scanlines_per_bucket);

    void processVertices(const ModelData* model);

    bool setupTriangleRasterization(const VertexData& vertex_1, const VertexData& vertex_2, const VertexData& vertex_3,
                                    RasterizationData<T>& rasterization, int16& y_start);

    bool setupTriangleRasterization(const Vector3<T>& v1_screen_pos, const Vector3<T>& v2_screen_pos,
                                    const Vector3<T>& v3_screen_pos, RasterizationData<T>& rasterization,
                                    int16& start_scanline);

    void rasterizeAndShadeTriangle(RasterizationData<T>& rasterization, const TriangleData& triangle, VertexData vertex, int16 first_scanline_y);

private:

    Framebuffer framebuffer;

    Depthbuffer depthbuffer;

    FrontFaceMode front_face_mode = COUNTERCLOCKWISE;

    NearPlaneType near_plane;

    ShaderProgram<T> shader_program;

    uint16 num_instances = 0;

    const ModelData* const* instance_models = nullptr;

    const InstanceData* const* instance_datas = nullptr;






};

} // namespace MicroRenderer

#include "Renderer.tpp"
