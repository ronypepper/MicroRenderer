//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Shading/ShaderProgram.h"

namespace MicroRenderer {

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setFramebuffer(void* address) requires(t_cfg.shader_cfg.shading == SHADING_ENABLED)
{
    framebuffer.setBuffer(address);
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
typename Renderer<T, t_cfg, ShaderProgram>::Framebuffer& Renderer<T, t_cfg, ShaderProgram>::getFramebuffer() requires(t_cfg.shader_cfg.shading == SHADING_ENABLED)
{
    return framebuffer;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setDepthbuffer(void* address) requires(t_cfg.shader_cfg.depth_test == DEPTH_TEST_ENABLED)
{
    depthbuffer.setBuffer(address);
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
typename Renderer<T, t_cfg, ShaderProgram>::Depthbuffer& Renderer<T, t_cfg, ShaderProgram>::getDepthbuffer() requires(t_cfg.shader_cfg.depth_test == DEPTH_TEST_ENABLED)
{
    return depthbuffer;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setResolution(int32 width, int32 height)
{
    assert(width >= 0 && height >= 0);
    int32 tex_height = 1;
    if constexpr (t_cfg.render_mode == FRAMEBUFFER) {
        tex_height = height;
    }
    if constexpr (t_cfg.shader_cfg.shading == SHADING_ENABLED) {
        framebuffer.setResolution(width, tex_height);
    }
    if constexpr (t_cfg.shader_cfg.depth_test == DEPTH_TEST_ENABLED) {
        depthbuffer.setResolution(width, tex_height);
    }
    width_minus_one = width - 1;
    height_minus_one = height - 1;

    // Compute clip screen borders.
    right_x_clip = static_cast<T>(-0.5) + static_cast<T>(width);
    top_y_clip = static_cast<T>(-0.5) + static_cast<T>(height);
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setNearPlane(T distance)
{
    if constexpr (t_cfg.shader_cfg.projection == PERSPECTIVE) {
        near_plane = distance;
        shader_program.setNearPlane(distance);
    }
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
ShaderProgram<T, t_cfg.shader_cfg>& Renderer<T, t_cfg, ShaderProgram>::getShaderProgram()
{
    return shader_program;
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setGlobalData(const GlobalData* global_data)
{
    shader_program.setGlobalData(global_data);
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setModels(const ModelData* models)
{
    this->models = models;
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setInstances(const InstanceData* instances, uint16 number)
{
    this->instances = instances;
    num_instances = number;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setVertexBuffers(VertexBuffer* buffers)
{
    vertex_buffers = buffers;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::setRasterizationBuffers(RasterizationBuffer* buffers, RasterizationOrder* order) requires(t_cfg.render_mode == SCANLINE)
{
    scanline_render_data.buffers = buffers;
    scanline_render_data.order = order;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
{
    x0 = std::clamp(x0, static_cast<T>(-0.5), right_x_clip);
    y0 = std::clamp(y0, static_cast<T>(-0.5), top_y_clip);
    x1 = std::clamp(x1, static_cast<T>(-0.5), right_x_clip);
    y1 = std::clamp(y1, static_cast<T>(-0.5), top_y_clip);
    rasterizeLineDDAUnsafe(x0, y0, x1, y1, color);
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
{
    // All cases of dx = 0, dy = 0, and both dx = 0 and dy = 0 are covered.
    if (y1 < y0) {
        // Always move in increasing y.
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    const T dy = y1 - y0;
    const T dx = x1 - x0;
    const T dx_abs = std::abs(dx);
    if (dx_abs >= dy) {
        // Line moves in x faster than in y.
        const auto x_start = static_cast<int32>(std::round(x0));
        const auto x_end = static_cast<int32>(std::round(x1));
        const T y_step = dy / dx_abs;
        T y = y0;
        T y_thresh = std::round(y);
        framebuffer.setCursor(x_start, static_cast<int32>(y_thresh));
        y_thresh += static_cast<T>(0.5);
        if (dx_abs == dx) {
            // Line moves to the right.
            for (int32 x = x_start + 1; x <= x_end; ++x) {
                y += y_step;
                if (y >= y_thresh) {
                    y_thresh += static_cast<T>(1.0);
                    framebuffer.moveCursorDown();
                }
                framebuffer.moveCursorRight();
                framebuffer.drawPixelAtCursor(color);
            }
        }
        else {
            // Line moves to the left.
            for (int32 x = x_start - 1; x >= x_end; --x) {
                y += y_step;
                if (y >= y_thresh) {
                    y_thresh += static_cast<T>(1.0);
                    framebuffer.moveCursorDown();
                }
                framebuffer.moveCursorLeft();
                framebuffer.drawPixelAtCursor(color);
            }
        }
    }
    else {
        // Line moves in x slower than in y.
        const auto y_start = static_cast<int32>(std::round(y0));
        const auto y_end = static_cast<int32>(std::round(y1));
        const T x_step = dx_abs / dy;
        T x = x0;
        T x_thresh = std::round(x);
        framebuffer.setCursor(static_cast<int32>(x_thresh), y_start);
        if (dx == dx_abs) {
            // Line moves to the right.
            x_thresh += static_cast<T>(0.5);
            for (int32 y = y_start + 1; y <= y_end; ++y) {
                x += x_step;
                if (x >= x_thresh) {
                    x_thresh += static_cast<T>(1.0);
                    framebuffer.moveCursorRight();
                }
                framebuffer.moveCursorDown();
                framebuffer.drawPixelAtCursor(color);
            }
        }
        else {
            // Line moves to the left.
            x_thresh -= static_cast<T>(0.5);
            for (int32 y = y_start + 1; y <= y_end; ++y) {
                x -= x_step;
                if (x_thresh >= x) {
                    x_thresh -= static_cast<T>(1.0);
                    framebuffer.moveCursorLeft();
                }
                framebuffer.moveCursorDown();
                framebuffer.drawPixelAtCursor(color);
            }
        }
    }
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c)
{
    // RasterizationBuffer rasterization;
    // int16 y_start = -1;
    // if (setupTriangleRasterization({a, 0}, {b, 0}, {c, 0}, rasterization, y_start)) {
    //     TriangleSource tri_source = {{1.f}};
    //     TriangleData triangle = {&tri_source, nullptr};
    //     VertexBuffer vertex_buffer =
    //     shadeFullTriangle(rasterization, triangle, y_start);
    // }
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::render()
{
    if constexpr (t_cfg.render_mode == SCANLINE) {
        // Reset scanline render data.
        scanline_render_data.num_buffers = 0;
        scanline_render_data.actives_order_start = 0;
        scanline_render_data.actives_order_stop = 0;
        scanline_render_data.next_scanline = 0;
    }

    // Process instances sequentially.
    for (uint16 instance_idx = 0; instance_idx < num_instances; ++instance_idx) {
        // Get model data.
        const ModelData* model = models + instances[instance_idx].model_idx;

        // Set instance data.
        shader_program.setInstanceData(instances + instance_idx);

        // Process vertices.
        processVertices(model);

        if constexpr (t_cfg.render_mode == SCANLINE) {
            // Temporarily store instance reference for later storage in rasterization buffers.
            scanline_render_data.instance_idx_marker = instance_idx;
        }

        // Process triangles.
        // Shading mode 'Framebuffer' shades here.
        // Shading mode 'SCANLINE' stores rasterization buffers for later line-by-line rasterization.
        for (uint32 tri_idx = 0; tri_idx < static_cast<uint32>(model->num_triangles); ++tri_idx) {
            cullAndClipTriangle(model, tri_idx);
        }
    }

    if constexpr (t_cfg.render_mode == SCANLINE) {
        // Sort rasterization buffers in y via the rasterization order.
        std::sort(scanline_render_data.order, scanline_render_data.order + scanline_render_data.num_buffers);
    }
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::renderNextScanline() requires (t_cfg.render_mode == SCANLINE)
{
    ScanlineRenderData& data = scanline_render_data;
    int32 scanline = scanline_render_data.next_scanline;

    // Add newly visible triangles to active section.
    while (data.actives_order_stop < data.num_buffers) {
        if (data.order[data.actives_order_stop].scanline == scanline) {
            // Add next triangle to actives.
            ++data.actives_order_stop;
        }
        else {
            break;
        }
    }

    // Shade all active triangles on scanline.
    for (uint16 i = data.actives_order_start; i < data.actives_order_stop; ++i) {
        RasterizationBuffer& rasterization = data.buffers[data.order[i].buffer_idx];

        // Set instance data.
        shader_program.setInstanceData(instances + rasterization.instance_idx);

        // Shade triangle on scanline.
        shadeScanlineOfTriangle(rasterization, scanline);

        // Check if half-triangle has ended on this scanline.
        if (static_cast<int32>(rasterization.y_halftri_end) == scanline) {
            if (rasterization.y_halftri_end == rasterization.y_fulltri_end) {
                // Triangle has ended completely. Remove triangle from active section.
                std::swap(data.order[i], data.order[data.actives_order_start]);
                ++data.actives_order_start;
            }
            else {
                // Adjust rasterization data for second half-triangle.
                if (rasterization.last_is_left) {
                    // Left is middle.
                    rasterization.left_x = rasterization.last_x;
                    rasterization.left_dx_per_dy = rasterization.last_dx_per_dy;
                    // Right edge has already been advanced to next scanline by last shadeScanlineOfTriangle() call.
                }
                else {
                    // Right is middle.
                    rasterization.right_x = rasterization.last_x;
                    rasterization.right_dx_per_dy = rasterization.last_dx_per_dy;
                    // Left edge has already been advanced to next scanline by last shadeScanlineOfTriangle() call.
                }
                rasterization.y_halftri_end = rasterization.y_fulltri_end;
            }
        }
    }

    // Increment scanline counter.
    ++scanline_render_data.next_scanline;
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::processVertices(const ModelData* model)
{
    for (uint16 vertex_idx = 0; vertex_idx < model->num_vertices; ++vertex_idx) {
        const VertexData vertex_data = {model->vertices + vertex_idx, vertex_buffers + vertex_idx};

        // Shade vertex.
        shader_program.shadeVertex(vertex_data);

        // Homogenize vertex.
        if constexpr(t_cfg.shader_cfg.projection == PERSPECTIVE) {
            vertex_data.buffer->homogenizeVertex(near_plane);
        }
    }
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::cullAndClipTriangle(const ModelData* model, uint32 tri_idx)
{
    // Gather vertex data.
    const uint16 v1_idx = model->indices[tri_idx].vertex_1_idx;
    const uint16 v2_idx = model->indices[tri_idx].vertex_2_idx;
    const uint16 v3_idx = model->indices[tri_idx].vertex_3_idx;
    const VertexData vertices[3] = {
        {model->vertices + v1_idx, vertex_buffers + v1_idx},
        {model->vertices + v2_idx, vertex_buffers + v2_idx},
        {model->vertices + v3_idx, vertex_buffers + v3_idx}
    };
    const Vector3<T>& v1_screen_pos = vertices[0].buffer->screen_position;
    const Vector3<T>& v2_screen_pos = vertices[1].buffer->screen_position;
    const Vector3<T>& v3_screen_pos = vertices[2].buffer->screen_position;

    // Backface culling and early degenerate check.
    const Vector2<T> v1_to_v2 = {v2_screen_pos.x - v1_screen_pos.x, v2_screen_pos.y - v1_screen_pos.y};
    const Vector2<T> v1_to_v3 = {v3_screen_pos.x - v1_screen_pos.x, v3_screen_pos.y - v1_screen_pos.y};
    constexpr T DEGENERATE_THRESHOLD = static_cast<T>(0.001);
    const T area = v1_to_v2.x * v1_to_v3.y - v1_to_v2.y * v1_to_v3.x;
    if constexpr (t_cfg.front_face == COUNTERCLOCKWISE) {
        if (area < DEGENERATE_THRESHOLD) // Counter-clockwise vertex order yields positive area.
            return;
    }
    else {
        if (area > DEGENERATE_THRESHOLD) // Clockwise vertex order yields negative area.
            return;
    }

    // Lambda for computing the outcode of a vertex for Cohen-Sutherland clipping.
    auto getCohenSutherlandOutcode = [right_x_clip = this->right_x_clip, top_y_clip = this->top_y_clip]
                                     (const Vector3<T>& position) -> uint8
    {
        constexpr T left_x_clip = static_cast<T>(-0.5);
        constexpr T bottom_y_clip = static_cast<T>(-0.5);

        constexpr uint8 OUTCODE_INSIDE = 0b0000;
        constexpr uint8 OUTCODE_LEFT   = 0b0001;
        constexpr uint8 OUTCODE_RIGHT  = 0b0010;
        constexpr uint8 OUTCODE_BOTTOM = 0b0100;
        constexpr uint8 OUTCODE_TOP    = 0b1000;

        uint8 outcode = OUTCODE_INSIDE;
        if (position.x < left_x_clip)
            outcode |= OUTCODE_LEFT;
        else if (position.x > right_x_clip)
            outcode |= OUTCODE_RIGHT;
        if (position.y < bottom_y_clip)
            outcode |= OUTCODE_BOTTOM;
        else if (position.y > top_y_clip)
            outcode |= OUTCODE_TOP;

        return outcode;
    };

    if constexpr(t_cfg.shader_cfg.culling == CULL_AT_SCREEN_BORDER) {
        // Skip triangles if certainly invisible according to the Cohen-Sutherland algorithm (no clipping).
        if ((getCohenSutherlandOutcode(v1_screen_pos) & getCohenSutherlandOutcode(v2_screen_pos) &
            getCohenSutherlandOutcode(v3_screen_pos)) != 0) {
            return;
        }
    }

    if constexpr(t_cfg.shader_cfg.clipping == CLIP_AT_NEAR_PLANE) {
        // Check if triangle is visible in z and clipped.
        T v1_clip_depth = vertices[0].buffer->getClipPosition().z;
        T v2_clip_depth = vertices[1].buffer->getClipPosition().z;
        T v3_clip_depth = vertices[2].buffer->getClipPosition().z;
        const uint8 v1_z_visible = v1_clip_depth >= static_cast<T>(0.0);
        const uint8 v2_z_visible = v2_clip_depth >= static_cast<T>(0.0);
        const uint8 v3_z_visible = v3_clip_depth >= static_cast<T>(0.0);
        const uint8 num_visible_verts = v1_z_visible + v2_z_visible + v3_z_visible;
        if (num_visible_verts) {
            // Triangle is at least partially visible.
            if (num_visible_verts == 3) {
                // Triangle fully visible (in z).
                processTriangle(tri_idx, vertices[0], vertices[1], vertices[2]);
            }
            else {
                // Temporary data for vertices created by clipping.
                VertexSource clipped_sources[2];
                VertexBuffer clipped_buffers[2];
                VertexData clipped_vertices[2] = {
                    {clipped_sources, clipped_buffers}, {clipped_sources + 1, clipped_buffers + 1}
                };

                if (num_visible_verts == 2) {
                    // Triangle is clipped into two new triangles.
                    const uint8 idx_1 = !v1_z_visible ? 0 : !v2_z_visible ? 1 : 2;
                    const uint8 idx_2 = idx_1 == 2 ? 0 : idx_1 + 1;
                    const uint8 idx_3 = idx_2 == 2 ? 0 : idx_2 + 1;
                    shader_program.interpolateVertices(vertices[idx_2], vertices[idx_1], clipped_sources, clipped_buffers);
                    shader_program.interpolateVertices(vertices[idx_3], vertices[idx_1], clipped_sources + 1, clipped_buffers + 1);
                    processTriangle(tri_idx, vertices[idx_3], clipped_vertices[1], clipped_vertices[0]);
                    processTriangle(tri_idx, vertices[idx_3], clipped_vertices[0], vertices[idx_2]);
                }
                else {
                    // Triangle is clipped into one new triangle.
                    const uint8 idx_1 = v1_z_visible ? 0 : v2_z_visible ? 1 : 2;
                    const uint8 idx_2 = idx_1 == 2 ? 0 : idx_1 + 1;
                    const uint8 idx_3 = idx_2 == 2 ? 0 : idx_2 + 1;
                    shader_program.interpolateVertices(vertices[idx_2], vertices[idx_1], clipped_sources, clipped_buffers);
                    shader_program.interpolateVertices(vertices[idx_3], vertices[idx_1], clipped_sources + 1, clipped_buffers + 1);
                    processTriangle(tri_idx, vertices[idx_1], clipped_vertices[0], clipped_vertices[1]);
                }
            }
        }
    }
    else {
        processTriangle(tri_idx, vertices[0], vertices[1], vertices[2]);
    }
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
bool Renderer<T, t_cfg, ShaderProgram>::setupTriangleRasterization(uint32 tri_idx, const VertexData& v1, const VertexData& v2,
                                                                   const VertexData& v3, RasterizationBuffer& rasterization, int32& start_scanline)
{
    // Triangle is split into first and second half-triangle, which are rasterized/shaded separately.
    // First half: scanlines contained between the two edges closest to y=0.
    // Second half: scanlines contained between the two edges farmost of y=0.

    // Swizzle vertex order to be counter-clockwise when front-facing.
    const Vector3<T>* positions[3];
    positions[0] = &v1.buffer->screen_position;
    if (t_cfg.front_face == COUNTERCLOCKWISE) {
        positions[1] = &v2.buffer->screen_position;
        positions[2] = &v3.buffer->screen_position;
    }
    else {
        positions[1] = &v3.buffer->screen_position;
        positions[2] = &v2.buffer->screen_position;
    }

    // Sort vertices in y, starting with smallest y.
    uint16 start_idx = 0;
    uint16 middle_idx = 1;
    uint16 end_idx = 2;
    if (positions[start_idx]->y > positions[end_idx]->y)
        std::swap(start_idx, end_idx);
    if (positions[start_idx]->y > positions[middle_idx]->y)
        std::swap(start_idx, middle_idx);
    if (positions[middle_idx]->y > positions[end_idx]->y)
        std::swap(middle_idx, end_idx);

    // Check half-triangle size along y and draw only those that are thick enough to avoid zero-divisions.
    const T dy_start_middle = positions[middle_idx]->y - positions[start_idx]->y;
    const T dy_middle_end = positions[end_idx]->y - positions[middle_idx]->y;
    constexpr T MIN_DELTA_Y = static_cast<T>(0.001); //std::numeric_limits<T>::min();
    if (dy_start_middle < MIN_DELTA_Y && dy_middle_end < MIN_DELTA_Y) {
        // Both half-triangles are too lean in y. Do not draw anything.
        return false;
    }

    // Shift left and flat-top edges by a small amount to deterministically draw edges going exactly through pixels.
    // This is called the "top left" fill rule.
    constexpr T TOPLEFT_EDGE_SHIFT = static_cast<T>(0.00001);

    uint16 peak_idx, left_idx, right_idx;
    T y_start;
    if (dy_start_middle < MIN_DELTA_Y) {
        // First half-triangle is too lean in y. Only draw second half-triangle.
        peak_idx = end_idx;
        left_idx = peak_idx == 2 ? 0 : peak_idx + 1;
        right_idx = left_idx == 2 ? 0 : left_idx + 1;

        /// Compute edge start x-coordinates.
        rasterization.left_x = positions[left_idx]->x + TOPLEFT_EDGE_SHIFT; // Shift left edge slightly.
        rasterization.right_x = positions[right_idx]->x;

        // Compute half-triangle start and end y-coordinates.
        y_start = std::min(positions[left_idx]->y, positions[right_idx]->y) + TOPLEFT_EDGE_SHIFT; // Shift top edge slightly.
        const T y_end = positions[peak_idx]->y;
        rasterization.y_halftri_end = static_cast<int16>(std::floor(y_end));
        rasterization.y_fulltri_end = rasterization.y_halftri_end;
    }
    else {
        // At least the first half-triangle is drawn.
        peak_idx = start_idx;
        right_idx = peak_idx == 2 ? 0 : peak_idx + 1;
        left_idx = right_idx == 2 ? 0 : right_idx + 1;

        // Compute edge start x-coordinates.
        rasterization.left_x = positions[peak_idx]->x + TOPLEFT_EDGE_SHIFT; // Shift left edge slightly.
        rasterization.right_x = positions[peak_idx]->x;

        // Compute (half-)triangle start and end y-coordinates.
        y_start = positions[peak_idx]->y;
        if (dy_middle_end < MIN_DELTA_Y) {
            // Second half-triangle is too lean in y. Only draw first half-triangle.
            const T y_end = std::min(positions[left_idx]->y, positions[right_idx]->y);
            rasterization.y_halftri_end = static_cast<int16>(std::floor(y_end));
            rasterization.y_fulltri_end = rasterization.y_halftri_end;
        }
        else {
            // Draw potentially both half-triangles.
            const T y_end = positions[end_idx]->y;
            const T y_middle = positions[middle_idx]->y;
            rasterization.y_halftri_end = static_cast<int16>(std::floor(y_middle));
            if (rasterization.y_halftri_end < static_cast<int16>(height_minus_one)) {
                // Draw both half-triangles.
                rasterization.y_fulltri_end = static_cast<int16>(std::floor(y_end));

                // Compute second half-triangle changed edge slope and x-start coordinate.
                rasterization.last_dx_per_dy = (positions[end_idx]->x - positions[middle_idx]->x) / (positions[end_idx]->y - positions[middle_idx]->y);
                const T middle_offset = rasterization.y_halftri_end + static_cast<T>(1.0) - y_middle;
                rasterization.last_x = positions[middle_idx]->x + rasterization.last_dx_per_dy * middle_offset;
                if (middle_idx == left_idx) {
                    rasterization.last_x += TOPLEFT_EDGE_SHIFT; // Shift left edge slightly.
                    rasterization.last_is_left = true;
                }
                else {
                    rasterization.last_is_left = false;
                }
            }
            else {
                // Skip second half-triangle, since it is fully above screen.
                rasterization.y_fulltri_end = rasterization.y_halftri_end;
            }
        }
    }

    // Compute edge slopes.
    const T dx_peak_left = positions[left_idx]->x - positions[peak_idx]->x;
    const T dy_peak_left = positions[left_idx]->y - positions[peak_idx]->y;
    rasterization.left_dx_per_dy = dx_peak_left / dy_peak_left;
    const T dx_peak_right = positions[right_idx]->x - positions[peak_idx]->x;
    const T dy_peak_right = positions[right_idx]->y - positions[peak_idx]->y;
    rasterization.right_dx_per_dy = dx_peak_right / dy_peak_right;

    // Compute start scanline.
    T first_scanline = std::ceil(y_start);
    start_scanline = static_cast<int32>(first_scanline);

    // Adjust edge positions to start scanline.
    // This adjustment is actually wrong by TOPLEFT_EDGE_SHIFT in case of only drawing second triangle. Does not matter however.
    const T start_offset = first_scanline - y_start;
    rasterization.left_x += rasterization.left_dx_per_dy * start_offset;
    rasterization.right_x += rasterization.right_dx_per_dy * start_offset;

    // Check cases that half-triangles do not even cover one scanline.
    // This may happen due to the rounding logic above, but the difference is always only one scanline.
    // It is important this is done after the above adjustment of edge positions.
    if (start_scanline > static_cast<int32>(rasterization.y_halftri_end)) {
        // Upper half-triangle does not even cover one scanline.
        if (rasterization.y_halftri_end == rasterization.y_fulltri_end) {
            // Only one half-triangle at all or two half-triangles that both do not even cover one scanline.
            // Skip triangle.
            return false;
        }

        // Switch directly to second half-triangle.
        // Second half-triangle starts at rasterization.y_halftri_end + 1, which is start_scanline. The edge that
        // does not get replaced doesn't need to be adjusted, since it already started on start_scanline.
        if (rasterization.last_is_left) {
            // Left is middle.
            rasterization.left_x = rasterization.last_x;
            rasterization.left_dx_per_dy = rasterization.last_dx_per_dy;
        }
        else {
            // Right is middle.
            rasterization.right_x = rasterization.last_x;
            rasterization.right_dx_per_dy = rasterization.last_dx_per_dy;
        }
        rasterization.y_halftri_end = rasterization.y_fulltri_end;
    }

    // If triangle is partially below screen, adjust rasterization data to start inside screen.
    if (start_scanline < 0) {
        if (rasterization.y_halftri_end < 0) {
            // Skip first half-triangle since it is fully outside screen.
            if (rasterization.y_fulltri_end < 0) {
                // Skip complete triangle since it is fully outside screen.
                return false;
            }

            // Advance second-half triangle scanline 0.
            if (rasterization.last_is_left) {
                // Left is middle.
                rasterization.left_x = rasterization.last_x;
                rasterization.left_dx_per_dy = rasterization.last_dx_per_dy;
                rasterization.left_x -= rasterization.left_dx_per_dy * static_cast<T>(rasterization.y_halftri_end + 1);
                rasterization.right_x -= rasterization.right_dx_per_dy * first_scanline;
            }
            else {
                // Right is middle.
                rasterization.right_x = rasterization.last_x;
                rasterization.right_dx_per_dy = rasterization.last_dx_per_dy;
                rasterization.left_x -= rasterization.left_dx_per_dy * first_scanline;
                rasterization.right_x -= rasterization.right_dx_per_dy * static_cast<T>(rasterization.y_halftri_end + 1);
            }
            rasterization.y_halftri_end = rasterization.y_fulltri_end;
        }
        else {
            // Skip scanlines of first half-triangle that are outside screen.
            rasterization.left_x -= rasterization.left_dx_per_dy * first_scanline;
            rasterization.right_x -= rasterization.right_dx_per_dy * first_scanline;
        }

        // The new start scanline is 0.
        start_scanline = 0;
        // Just a reminder: first_scanline = static_cast<T>(0.0);
    }

    // Setup triangle and store initial interpolation position (prev_scanline_stop_x and start_scanline).
    rasterization.prev_scanline_stop_x = static_cast<int16>(std::floor(rasterization.right_x));
    shader_program.setupTriangle(tri_idx, v1, v2, v3, &rasterization.triangle_buffer,
                                 rasterization.prev_scanline_stop_x, start_scanline);

    return true;
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
template <typename BufferType>
typename BufferType::BufferPosition Renderer<T, t_cfg, ShaderProgram>::getPositionInBuffer(BufferType buffer, int32 x, int32 y)
{
    if constexpr (t_cfg.render_mode == FRAMEBUFFER) {
        return buffer.getWrappedBufferPosition(x, y);
    }
    else if constexpr (t_cfg.render_mode == SCANLINE) {
        return buffer.pixelNumToBufferPosition(x);
    }
    return {};
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::shadeScanlineOfTriangle(RasterizationBuffer& rasterization, int32 scanline)
{
    TriangleBuffer* triangle = &rasterization.triangle_buffer;

    // Compute start and end pixels of scanline.
    const int32 x_start = std::max(static_cast<int32>(std::ceil(rasterization.left_x)), static_cast<int32>(0));
    const int32 x_stop = std::min(static_cast<int32>(std::floor(rasterization.right_x)), width_minus_one);
    if (x_start <= x_stop) {
        // Interpolate in x to first pixel on scanline.
        int32 initial_offset = x_start - static_cast<int32>(rasterization.prev_scanline_stop_x);
        shader_program.template interpolateAttributes<IncrementationMode::OffsetInX>(triangle, initial_offset);

        // Store end of scanline for interpolation in x at next scanline.
        rasterization.prev_scanline_stop_x = x_stop;

        // Perform depth-test and shading of pixels on scanline, if enabled.
        if constexpr(t_cfg.shader_cfg.shading == SHADING_DISABLED && t_cfg.shader_cfg.depth_test == DEPTH_TEST_ENABLED) {
            auto depthbuffer_position = getPositionInBuffer(depthbuffer, x_start, scanline);
            if (triangle->depth.getValue() > depthbuffer.readPixelAt(depthbuffer_position)) {
                depthbuffer.drawPixelAt(depthbuffer_position, triangle->depth.getValue());
            }
            for (int32 x = x_start; x < x_stop; ++x) {
                shader_program.template interpolateAttributes<IncrementationMode::OneInX>(triangle);
                depthbuffer.moveBufferPositionRight(depthbuffer_position);
                if (triangle->depth.getValue() > depthbuffer.readPixelAt(depthbuffer_position)) {
                    depthbuffer.drawPixelAt(depthbuffer_position, triangle->depth.getValue());
                }
            }
        }
        else if constexpr(t_cfg.shader_cfg.shading == SHADING_ENABLED && t_cfg.shader_cfg.depth_test == DEPTH_TEST_DISABLED) {
            auto framebuffer_position = getPositionInBuffer(framebuffer, x_start, scanline);
            framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(triangle));
            for (int32 x = x_start; x < x_stop; ++x) {
                shader_program.template interpolateAttributes<IncrementationMode::OneInX>(triangle);
                framebuffer.moveBufferPositionRight(framebuffer_position);
                framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(triangle));
            }
        }
        else if constexpr(t_cfg.shader_cfg.shading == SHADING_ENABLED && t_cfg.shader_cfg.depth_test == DEPTH_TEST_ENABLED) {
            auto framebuffer_position = getPositionInBuffer(framebuffer, x_start, scanline);
            auto depthbuffer_position = getPositionInBuffer(depthbuffer, x_start, scanline);
            if (triangle->depth.getValue() > depthbuffer.readPixelAt(depthbuffer_position)) {
                depthbuffer.drawPixelAt(depthbuffer_position, triangle->depth.getValue());
                framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(triangle));
            }
            for (int32 x = x_start; x < x_stop; ++x) {
                shader_program.template interpolateAttributes<IncrementationMode::OneInX>(triangle);
                framebuffer.moveBufferPositionRight(framebuffer_position);
                depthbuffer.moveBufferPositionRight(depthbuffer_position);
                if (triangle->depth.getValue() > depthbuffer.readPixelAt(depthbuffer_position)) {
                    depthbuffer.drawPixelAt(depthbuffer_position, triangle->depth.getValue());
                    framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(triangle));
                }
            }
        }
    }

    // Compute x-coords of edges at next scanline.
    rasterization.left_x += rasterization.left_dx_per_dy;
    rasterization.right_x += rasterization.right_dx_per_dy;

    // Interpolate in y to next scanline.
    shader_program.template interpolateAttributes<IncrementationMode::OneInY>(triangle);
}

template<typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::shadeFullTriangle(RasterizationBuffer& rasterization, int32 start_scanline)
{
    // Triangle is split into first and second half-triangle, which are rasterized/shaded separately.
    // First half: scanlines contained between the two edges closest to y=0.
    // Second half: scanlines contained between the two edges farmost of y=0.

    // Lambda for rasterizing half a triangle.
    auto rasterizeHalfTriangle = [&rasterization, this](int32 y_start) {
        const int32 y_end = std::min(static_cast<int32>(rasterization.y_halftri_end), height_minus_one);
        for (int32 y = y_start; y <= y_end; ++y) {
            shadeScanlineOfTriangle(rasterization, y);
        }
    };

    // Rasterize first half-triangle.
    rasterizeHalfTriangle(start_scanline);
    if (rasterization.y_halftri_end == rasterization.y_fulltri_end) {
        // No second half-triangle exists.
        return;
    }

    // Adjust rasterization data for second half-triangle.
    if (rasterization.last_is_left) {
        // Left is middle.
        rasterization.left_x = rasterization.last_x;
        rasterization.left_dx_per_dy = rasterization.last_dx_per_dy;
        // Right edge has already been advanced to next scanline by last shadeScanlineOfTriangle() call.
    }
    else {
        // Right is middle.
        rasterization.right_x = rasterization.last_x;
        rasterization.right_dx_per_dy = rasterization.last_dx_per_dy;
        // Left edge has already been advanced to next scanline by last shadeScanlineOfTriangle() call.
    }
    start_scanline = static_cast<int32>(rasterization.y_halftri_end) + 1;
    rasterization.y_halftri_end = rasterization.y_fulltri_end;

    // Rasterize second half-triangle.
    rasterizeHalfTriangle(start_scanline);
}

template <typename T, RendererConfiguration t_cfg, template <typename, ShaderConfiguration> class ShaderProgram>
void Renderer<T, t_cfg, ShaderProgram>::processTriangle(uint32 tri_idx, VertexData v1, VertexData v2, VertexData v3)
{
    if constexpr (t_cfg.render_mode == FRAMEBUFFER) {
        // Setup triangle and rasterization.
        RasterizationBuffer rasterization;
        int32 start_scanline;
        if (setupTriangleRasterization(tri_idx, v1, v2, v3, rasterization, start_scanline)) {
            // Shade.
            shadeFullTriangle(rasterization, start_scanline);
        }
    }
    else if constexpr (t_cfg.render_mode == SCANLINE) {
        // Get reference to next entry in stored rasterization buffers.
        uint16 buffer_idx = scanline_render_data.num_buffers;
        RasterizationBuffer& rasterization = scanline_render_data.buffers[buffer_idx];

        // Setup triangle and rasterization.
        int32 start_scanline;
        if (setupTriangleRasterization(tri_idx, v1, v2, v3, rasterization, start_scanline)) {
            // Store instance reference in rasterization buffer.
            rasterization.instance_idx = scanline_render_data.instance_idx_marker;

            // Add entry to stored rasterization order.
            RasterizationOrder& order = scanline_render_data.order[buffer_idx];
            order.scanline = static_cast<int16>(start_scanline);
            order.buffer_idx = buffer_idx;

            // Increment used rasterization buffers counter.
            ++scanline_render_data.num_buffers;
        }
    }
}

} // namespace MicroRenderer
