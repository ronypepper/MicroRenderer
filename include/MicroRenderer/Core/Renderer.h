//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include "Framebuffer.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Math/Vector3.h"
#include "MicroRenderer/Core/Shading/ShaderInterface.h"

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
    T diz_per_dx;
    T diz_per_dy;
    T left_inverse_z;
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

template<typename T, class ShaderProgram>
struct RendererConfig {
    using value_type = T;
    using shader_program = ShaderProgram;
    ColorCoding color_coding = RGB888;
};

template<typename CfgType, CfgType Config>
class Renderer {
public:
    using T = typename CfgType::value_type;
    using ShaderProgram = typename CfgType::shader_program;
    using RendererFramebuffer = Framebuffer<T, Config.color_coding>;

    USE_SHADER_INTERFACE_TYPES(ShaderProgram::ShaderInterface_type)

    Renderer(uint32 width, uint32 height, uint8* frame_buffer);

    void setFramebuffer(uint8* address);

    RendererFramebuffer& getFramebuffer();

    void setDepthbuffer(float* address);

    float* getDepthbuffer() const;

    void setFrontFace(FrontFaceMode front_face);

    ShaderProgram& getShaderProgram();

    void setRenderInstances(uint16 number, const ModelData* const* models, const InstanceData* const* instances);

    void setShadingBuffers(VertexBuffer* vertex, TriangleBuffer* triangle);

    void setRasterizationBuffers(RasterizationData<T>* data, RasterizationOrder* order);

    void rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c);

    void renderFramebuffer();

    void renderScanlines();

    void renderScanbuckets(int16 scanlines_per_bucket);

    bool setupTriangleRasterization(const VertexData& vertex_1, const VertexData& vertex_2, const VertexData& vertex_3,
                                    RasterizationData<T>& rasterization, int16& y_start);

    bool setupTriangleRasterization(const Vector3<T>& v1_screen_pos, const Vector3<T>& v2_screen_pos,
                                    const Vector3<T>& v3_screen_pos, RasterizationData<T>& rasterization,
                                    int16& start_scanline);

    void rasterizeAndShadeTriangle(RasterizationData<T>& rasterization, const TriangleData& triangle,
                                   int16 first_scanline_y);

private:

    RendererFramebuffer framebuffer;

    float* depthbuffer = nullptr;

    FrontFaceMode front_face_mode = COUNTERCLOCKWISE;

    ShaderProgram shader_program;

    uint16 num_instances = 0;

    const ModelData* const* instance_models = nullptr;

    const InstanceData* const* instance_datas = nullptr;

    uint16 num_rasterization_candidates = 0;

    RasterizationData<T>* rasterization_data_buffer = nullptr;

    VertexBuffer* vertex_buffers = nullptr;

    TriangleBuffer* triangle_buffers = nullptr;

    int16 num_scanlines_per_bucket = 1;

    RasterizationOrder* rasterization_order_buffer = nullptr;

    uint16 order_buffer_actives_begin = 0;

    uint16 order_buffer_actives_end = 0;
};

} // namespace MicroRenderer

#include "Renderer.tpp"
