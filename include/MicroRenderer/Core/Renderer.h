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

enum RenderMode : uint32
{
    FRAMEBUFFER,
    SCANLINE,
    NUM_RENDER_MODES
};

enum FrontFace : uint32
{
    COUNTERCLOCKWISE,
    CLOCKWISE,
    NUM_FRONT_FACE_MODES
};

struct RendererConfiguration
{
    RenderMode render_mode;
    FrontFace front_face;
};

template<typename T, RendererConfiguration t_cfg, template <typename> class ShaderProgram>
class Renderer {
    static_assert(t_cfg.render_mode < NUM_RENDER_MODES, "Renderer: Invalid render mode in configuration!");
    static_assert(t_cfg.front_face < NUM_FRONT_FACE_MODES, "Renderer: Invalid front face mode in configuration!");
public:
    USE_SHADER_INTERFACE(ShaderProgram<T>::ShaderInterface);
    static constexpr ShaderProgramConfig shader_cfg = ShaderProgram<T>::configuration;
    static constexpr TextureConfig framebuffer_cfg = {
        ACCESS_READWRITE, ShaderInterface::shader_output.format, SWIZZLE_NONE, ShaderInterface::shader_output.type,
        WRAPMODE_NONE
    };
    static constexpr TextureConfig depthbuffer_cfg = {
        ACCESS_READWRITE, FORMAT_DEPTH, SWIZZLE_NONE, TYPE_DECIMAL, WRAPMODE_NONE
    };
    using Framebuffer = std::conditional_t<shader_cfg.shading == SHADING_ENABLED, Texture2D<T, framebuffer_cfg>, std::monostate>;
    using Depthbuffer = std::conditional_t<shader_cfg.depth_test == DEPTH_TEST_ENABLED, Texture2D<T, depthbuffer_cfg>, std::monostate>;
    using NearPlaneType = std::conditional_t<shader_cfg.projection == PERSPECTIVE, T, std::monostate>;
    struct RenderInstruction
    {
        uint16 model_idx = 0;
        InstanceData instance_data;
    };
    struct RasterizationBuffer
    {
        bool last_is_left;
        uint16 instruction_idx;
        int16 prev_scanline_stop_x;
        int16 y_halftri_end;
        int16 y_fulltri_end;
        T left_dx_per_dy;
        T right_dx_per_dy;
        T last_dx_per_dy;
        T left_x;
        T right_x;
        T last_x;
        TriangleBuffer triangle_buffer;
    };
    struct RasterizationOrder
    {
        int16 scanline;
        uint16 buffer_idx;

        friend bool operator<(const RasterizationOrder& lhs, const RasterizationOrder& rhs)
        {
            return lhs.scanline < rhs.scanline;
        }
    };
    struct ScanlineRenderData
    {
        RasterizationBuffer* buffers = nullptr;

        RasterizationOrder* order = nullptr;

        uint16 num_buffers = 0;

        uint16 actives_order_start = 0;

        uint16 actives_order_stop = 0;

        int16 num_scanlines_per_bucket = 1;

        uint16 instruction_idx_marker = 0;

        int32 next_scanline = 0;
    };
    using RenderData = std::conditional_t<t_cfg.render_mode == SCANLINE, ScanlineRenderData, std::monostate>;

    Renderer() = default;

    void setFramebuffer(void* address) requires(shader_cfg.shading == SHADING_ENABLED);

    Framebuffer& getFramebuffer() requires(shader_cfg.shading == SHADING_ENABLED);

    void setDepthbuffer(void* address) requires(shader_cfg.depth_test == DEPTH_TEST_ENABLED);

    Depthbuffer& getDepthbuffer() requires(shader_cfg.depth_test == DEPTH_TEST_ENABLED);

    void setResolution(int32 width, int32 height);

    void setNearPlane(T distance) requires(shader_cfg.projection == PERSPECTIVE);

    ShaderProgram<T>& getShaderProgram();

    void setGlobalData(const GlobalData* global_data);

    void setModels(const ModelData* models);

    void setRenderInstructions(const RenderInstruction* instructions, uint16 number);

    void setVertexBuffers(VertexBuffer* buffers);

    void setRasterizationBuffers(RasterizationBuffer* buffers, RasterizationOrder* order) requires(t_cfg.render_mode == SCANLINE);

    void rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T> &color);

    void rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c);
    
    void render();

    void renderNextScanline() requires(t_cfg.render_mode == SCANLINE);

private:
    void processVertices(const ModelData* model);

    void cullAndClipTriangle(const ModelData* model, uint16 tri_idx);

    bool setupTriangleRasterization(const VertexData& vertex_1, const VertexData& vertex_2, const VertexData& vertex_3,
                                    RasterizationBuffer& rasterization, int32& start_scanline);

    template<typename BufferType>
    typename BufferType::BufferPosition getPositionInBuffer(BufferType buffer, int32 x, int32 y);

    void shadeScanlineOfTriangle(RasterizationBuffer& rasterization, int32 scanline);

    void shadeFullTriangle(RasterizationBuffer& rasterization, int32 start_scanline);

    void processTriangle(VertexData vertex_1, VertexData vertex_2, VertexData vertex_3);

    Framebuffer framebuffer;

    Depthbuffer depthbuffer;

    int32 width_minus_one = 0;

    int32 height_minus_one = 0;

    NearPlaneType near_plane;

    T right_x_clip = static_cast<T>(-0.5);

    T top_y_clip = static_cast<T>(-0.5);

    ShaderProgram<T> shader_program;

    const ModelData* models = nullptr;

    const RenderInstruction* render_instructions = nullptr;

    uint16 num_instructions = 0;

    VertexBuffer* vertex_buffers = nullptr;

    RenderData scanline_render_data;
};

} // namespace MicroRenderer

#include "Renderer.tpp"
