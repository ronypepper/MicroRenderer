//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <functional>

#include "MicroRenderer/Math/Vector2.h"
#include "MicroRenderer/Core/Shading/ShaderProgram.h"

namespace MicroRenderer {

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setFramebuffer(void* address, uint32 width, uint32 height) requires(shader_config.shading == SHADING_ENABLED)
    {
        framebuffer.changeBuffer(static_cast<typename Framebuffer::BufferPointer>(address), width, height);
    }

    template<typename T, template <typename> class ShaderProgram>
    typename Renderer<T, ShaderProgram>::Framebuffer& Renderer<T, ShaderProgram>::getFramebuffer() requires(shader_config.shading == SHADING_ENABLED)
    {
        return framebuffer;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setDepthbuffer(void* address, uint32 width, uint32 height) requires(shader_config.depth_test == DEPTH_TEST_ENABLED)
    {
        depthbuffer.changeBuffer(static_cast<typename Depthbuffer::BufferPointer>(address), width, height);
    }

    template<typename T, template <typename> class ShaderProgram>
    typename Renderer<T, ShaderProgram>::Depthbuffer& Renderer<T, ShaderProgram>::getDepthbuffer() requires(shader_config.depth_test == DEPTH_TEST_ENABLED)
    {
        return depthbuffer;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setFrontFace(FrontFaceMode front_face)
    {
        front_face_mode = front_face;
    }

    template <typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setNearPlane(T distance) requires (shader_config.projection == PERSPECTIVE)
    {
        near_plane = distance;
        shader_program.setNearPlane(distance);
    }

    template<typename T, template <typename> class ShaderProgram>
    ShaderProgram<T>& Renderer<T, ShaderProgram>::getShaderProgram()
    {
        return shader_program;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setRenderInstances(uint16 number, const ModelData* const* models,
                                                       const InstanceData* const* instances)
    {
        num_instances = number;
        instance_models = models;
        instance_datas = instances;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setShadingBuffers(VertexBuffer* vertex, TriangleBuffer* triangle)
    {
        vertex_buffers = vertex;
        triangle_buffers = triangle;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::setRasterizationBuffers(RasterizationData<T>* data, RasterizationOrder* order)
    {
        rasterization_data_buffer = data;
        rasterization_order_buffer = order;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
    {
        x0 = std::clamp(x0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y0 = std::clamp(y0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        x1 = std::clamp(x1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y1 = std::clamp(y1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        rasterizeLineDDAUnsafe(x0, y0, x1, y1, color);
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
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

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c)
    {
        // RasterizationData<T> rasterization;
        // int16 y_start = -1;
        // if (setupTriangleRasterization({a, 0}, {b, 0}, {c, 0}, rasterization, y_start)) {
        //     TriangleSource tri_source = {{1.f}};
        //     TriangleData triangle = {&tri_source, nullptr};
        //     VertexBuffer vertex_buffer =
        //     rasterizeAndShadeTriangle(rasterization, triangle, y_start);
        // }
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::renderFramebuffer()
    {
        // Compute clip screen borders.
        const T left_x_clip = static_cast<T>(-0.5);
        const T right_x_clip = static_cast<T>(-0.5) + static_cast<T>(framebuffer.getWidth());
        const T bottom_y_clip = static_cast<T>(-0.5);
        const T top_y_clip = static_cast<T>(-0.5) + static_cast<T>(framebuffer.getHeight());

        // Render instances sequentially.
        for (uint16 instance_idx = 0; instance_idx < num_instances; ++instance_idx) {
            // Set instance data.
            const InstanceData* instance_data = instance_datas[instance_idx];
            shader_program.setInstanceData(instance_data);

            // Get model data.
            const ModelData* model = instance_models[instance_idx];

            // Vertex processing.
            processVertices(model);

            // Triangle processing.
            for (uint16 tri_idx = 0; tri_idx < model->num_triangles; ++tri_idx) {
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

                // Lambda for computing the outcode of a vertex for Cohen-Sutherland clipping.
                auto getCohenSutherlandOutcode = [&, this](const Vector3<T>& position) -> uint8 {
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

                if constexpr(shader_config.culling == CULL_AT_SCREEN_BORDER) {
                    // Skip triangles if certainly invisible according to the Cohen-Sutherland algorithm (no clipping).
                    if ((getCohenSutherlandOutcode(v1_screen_pos) & getCohenSutherlandOutcode(v2_screen_pos) &
                        getCohenSutherlandOutcode(v3_screen_pos)) != 0) {
                        continue;
                    }
                }

                // Temporary triangle rasterization/shading data.
                TriangleBuffer tri_buffer;
                TriangleData triangle = {model->triangles + tri_idx, &tri_buffer};
                RasterizationData<T> rasterization;
                int16 y_start;

                if constexpr(shader_config.clipping == CLIP_AT_NEAR_PLANE) {
                    // Check if triangle is visible in z and clipped.
                    T v1_clip_depth, v2_clip_depth, v3_clip_depth;
                    if constexpr(shader_config.projection == PERSPECTIVE) {
                        v1_clip_depth = vertices[0].buffer->clip_position.z;
                        v2_clip_depth = vertices[1].buffer->clip_position.z;
                        v3_clip_depth = vertices[2].buffer->clip_position.z;
                    }
                    else {
                        v1_clip_depth = v1_screen_pos.z;
                        v2_clip_depth = v2_screen_pos.z;
                        v3_clip_depth = v3_screen_pos.z;
                    }
                    const uint8 v1_z_visible = v1_clip_depth >= static_cast<T>(0.0);
                    const uint8 v2_z_visible = v2_clip_depth >= static_cast<T>(0.0);
                    const uint8 v3_z_visible = v3_clip_depth >= static_cast<T>(0.0);
                    const uint8 num_visible_verts = v1_z_visible + v2_z_visible + v3_z_visible;
                    if (num_visible_verts) {
                        // Triangle is at least partially visible.
                        if (num_visible_verts == 3) {
                            // Triangle fully visible (in z).
                            // Rasterize / shade.
                            shader_program.setupTriangle(vertices[0], vertices[1], vertices[2], triangle);
                            if (setupTriangleRasterization(vertices[0], vertices[1], vertices[2], rasterization, y_start)) {
                                rasterizeAndShadeTriangle(rasterization, triangle, vertices[0], y_start);
                            }
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

                                // Rasterize / shade.
                                shader_program.setupTriangle(vertices[idx_3], clipped_vertices[1], clipped_vertices[0], triangle);
                                if (setupTriangleRasterization(vertices[idx_3], clipped_vertices[1], clipped_vertices[0], rasterization, y_start)) {
                                    rasterizeAndShadeTriangle(rasterization, triangle, vertices[idx_3], y_start);
                                }
                                shader_program.setupTriangle(vertices[idx_3], clipped_vertices[0], vertices[idx_2], triangle);
                                if (setupTriangleRasterization(vertices[idx_3], clipped_vertices[0], vertices[idx_2], rasterization, y_start)) {
                                    rasterizeAndShadeTriangle(rasterization, triangle, vertices[idx_3], y_start);
                                }
                            }
                            else {
                                // Triangle is clipped into one new triangle.
                                const uint8 idx_1 = v1_z_visible ? 0 : v2_z_visible ? 1 : 2;
                                const uint8 idx_2 = idx_1 == 2 ? 0 : idx_1 + 1;
                                const uint8 idx_3 = idx_2 == 2 ? 0 : idx_2 + 1;
                                shader_program.interpolateVertices(vertices[idx_2], vertices[idx_1], clipped_sources, clipped_buffers);
                                shader_program.interpolateVertices(vertices[idx_3], vertices[idx_1], clipped_sources + 1, clipped_buffers + 1);

                                // Rasterize / shade.
                                shader_program.setupTriangle(vertices[idx_1], clipped_vertices[0], clipped_vertices[1], triangle);
                                if (setupTriangleRasterization(vertices[idx_1], clipped_vertices[0], clipped_vertices[1], rasterization, y_start)) {
                                    rasterizeAndShadeTriangle(rasterization, triangle, vertices[idx_1], y_start);
                                }
                            }
                        }
                    }
                }
                else {
                    // Rasterize / shade.
                    shader_program.setupTriangle(vertices[0], vertices[1], vertices[2], triangle);
                    if (setupTriangleRasterization(vertices[0], vertices[1], vertices[2], rasterization, y_start)) {
                        rasterizeAndShadeTriangle(rasterization, triangle, vertices[0], y_start);
                    }
                }
            }
        }
    }

    template <typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::processVertices(const ModelData* model)
    {
        for (uint16 vertex_idx = 0; vertex_idx < model->num_vertices; ++vertex_idx) {
            const VertexData vertex_data = {model->vertices + vertex_idx, vertex_buffers + vertex_idx};

            // Transform vertex.
            shader_program.transformVertex(vertex_data);

            // Shade vertex.
            shader_program.shadeVertex(vertex_data);

            // Homogenize vertex.
            if constexpr(shader_config.projection == PERSPECTIVE) {
                const T depth = vertex_data.buffer->clip_position.w;
                if (depth >= near_plane) {
                    const T inverse_depth = static_cast<T>(1.0) / depth;
                    vertex_data.buffer->clip_position.w = inverse_depth;
                    vertex_data.buffer->screen_position = vertex_data.buffer->clip_position.getXYZ() * inverse_depth;
                }
            }
        }
    }

    template<typename T, template <typename> class ShaderProgram>
    bool Renderer<T, ShaderProgram>::setupTriangleRasterization(const VertexData& vertex_1, const VertexData& vertex_2,
        const VertexData& vertex_3, RasterizationData<T>& rasterization, int16& y_start)
    {
        // TODO: maybe remove this function
        return setupTriangleRasterization(vertex_1.buffer->screen_position, vertex_2.buffer->screen_position,
                                          vertex_3.buffer->screen_position, rasterization, y_start);
    }

    template<typename T, template <typename> class ShaderProgram>
    bool Renderer<T, ShaderProgram>::setupTriangleRasterization(const Vector3<T>& v1_screen_pos,
                                                               const Vector3<T>& v2_screen_pos,
                                                               const Vector3<T>& v3_screen_pos,
                                                               RasterizationData<T>& rasterization, int16& start_scanline)
    {
        // Triangle is split into first and second half-triangle, which are rasterized/shaded separately.
        // First half: scanlines contained between the two edges closest to y=0.
        // Second half: scanlines contained between the two edges farmost of y=0.

        const Vector3<T>* positions[3];
        positions[0] = &v1_screen_pos;
        if (front_face_mode == COUNTERCLOCKWISE) {
            positions[1] = &v2_screen_pos;
            positions[2] = &v3_screen_pos;
        }
        else {
            positions[1] = &v3_screen_pos;
            positions[2] = &v2_screen_pos;
        }

        // Backface culling and degenerate check.
        const Vector2<T> v1_to_v2 = {positions[1]->x - positions[0]->x, positions[1]->y - positions[0]->y};
        const Vector2<T> v1_to_v3 = {positions[2]->x - positions[0]->x, positions[2]->y - positions[0]->y};
        constexpr T DEGENERATE_THRESHOLD = static_cast<T>(0.001);
        const T area = v1_to_v2.x * v1_to_v3.y - v1_to_v2.y * v1_to_v3.x; // Clockwise vertex order yields negative area.
        if (area < DEGENERATE_THRESHOLD)
            return false;

        // Sort vertices in y, starting with smallest y.
        uint8 start_idx = 0;
        uint8 middle_idx = 1;
        uint8 end_idx = 2;
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

        uint8 peak_idx, left_idx, right_idx;
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
            rasterization.y_halftri_end = std::floor(y_end);
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
                rasterization.y_halftri_end = std::floor(y_end);
                rasterization.y_fulltri_end = rasterization.y_halftri_end;
            }
            else {
                // Draw both half-triangles.
                const T y_end = positions[end_idx]->y;
                const T y_middle = positions[middle_idx]->y;
                rasterization.y_halftri_end = std::floor(y_middle);
                rasterization.y_fulltri_end = std::floor(y_end);

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
        }

        // Compute edge slopes.
        const T dx_peak_left = positions[left_idx]->x - positions[peak_idx]->x;
        const T dy_peak_left = positions[left_idx]->y - positions[peak_idx]->y;
        rasterization.left_dx_per_dy = dx_peak_left / dy_peak_left;
        const T dx_peak_right = positions[right_idx]->x - positions[peak_idx]->x;
        const T dy_peak_right = positions[right_idx]->y - positions[peak_idx]->y;
        rasterization.right_dx_per_dy = dx_peak_right / dy_peak_right;

        // Compute start scanline.
        start_scanline = std::ceil(y_start);

        // Adjust edge positions to start scanline.
        const T start_offset = start_scanline - y_start; // This adjustment is actually wrong by TOPLEFT_EDGE_SHIFT in case of only drawing second triangle. Does not matter however.
        const T left_x_adjustment = rasterization.left_dx_per_dy * start_offset;
        rasterization.left_x += left_x_adjustment;
        rasterization.right_x += rasterization.right_dx_per_dy * start_offset;

        return true;
    }

    template<typename T, template <typename> class ShaderProgram>
    void Renderer<T, ShaderProgram>::rasterizeAndShadeTriangle(RasterizationData<T>& rasterization,
                                                              const TriangleData& triangle, VertexData vertex,
                                                              int16 first_scanline_y)
    {
        // Triangle is split into first and second half-triangle, which are rasterized/shaded separately.
        // First half: scanlines contained between the two edges closest to y=0.
        // Second half: scanlines contained between the two edges farmost of y=0.

        // Lambda for rasterizing half a triangle.
        auto rasterizeHalfTriangle = [&rasterization, triangle, vertex, this](int16 y_start) {
            // Skip scanlines above screen.
            if (y_start < 0) {
                rasterization.left_x -= rasterization.left_dx_per_dy * static_cast<float>(y_start);
                rasterization.right_x -= rasterization.right_dx_per_dy * static_cast<float>(y_start);
                y_start = 0;
            }
            const int32 y_end = std::min(static_cast<int32>(rasterization.y_halftri_end), framebuffer.getHeight() - 1);
            for (int32 y = y_start; y <= y_end; ++y) {
                // Compute start and end pixels of scanline.
                const int32 x_start = std::max(static_cast<int32>(std::ceil(rasterization.left_x)), static_cast<int32>(0));
                const int32 x_stop = std::min(static_cast<int32>(std::floor(rasterization.right_x)), framebuffer.getWidth() - 1);
                if (x_start <= x_stop) {
                    // Initally interpolate fragment to first pixel on scanline.
                    Fragment fragment;
                    shader_program.interpolateTo(&fragment, triangle, vertex, x_start, y);

                    // Perform depth-test and shading of pixels on scanline, if enabled.
                    if constexpr(shader_config.shading == SHADING_DISABLED && shader_config.depth_test == DEPTH_TEST_ENABLED) {
                        auto depthbuffer_position = depthbuffer.getBufferPositionAt(x_start, y);
                        if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                            depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                        }
                        for (int32 x = x_start; x < x_stop; ++x) {
                            shader_program.interpolateRight(&fragment, triangle);
                            depthbuffer.moveBufferPositionRight(depthbuffer_position);
                            if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                                depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                            }
                        }
                    }
                    else if constexpr(shader_config.shading == SHADING_ENABLED && shader_config.depth_test == DEPTH_TEST_DISABLED) {
                        auto framebuffer_position = framebuffer.getBufferPositionAt(x_start, y);
                        framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                        for (int32 x = x_start; x < x_stop; ++x) {
                            shader_program.interpolateRight(&fragment, triangle);
                            framebuffer.moveBufferPositionRight(framebuffer_position);
                            framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                        }
                    }
                    else if constexpr(shader_config.shading == SHADING_ENABLED && shader_config.depth_test == DEPTH_TEST_ENABLED) {
                        auto framebuffer_position = framebuffer.getBufferPositionAt(x_start, y);
                        auto depthbuffer_position = depthbuffer.getBufferPositionAt(x_start, y);
                        if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                            depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                            framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                        }
                        for (int32 x = x_start; x < x_stop; ++x) {
                            shader_program.interpolateRight(&fragment, triangle);
                            framebuffer.moveBufferPositionRight(framebuffer_position);
                            depthbuffer.moveBufferPositionRight(depthbuffer_position);
                            if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                                depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                                framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                            }
                        }
                    }




                    // // Shade and draw pixels between edges on scanline.
                    // Fragment fragment;
                    // shader_program.interpolateTo(&fragment, triangle, vertex, x_start, y);
                    // typename Framebuffer::BufferPosition framebuffer_position;
                    // typename Depthbuffer::BufferPosition depthbuffer_position;
                    // if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //     framebuffer_position = framebuffer.getBufferPositionAt(x_start, y);
                    // }
                    // if constexpr(shader_config.depth_test == DEPTH_TEST_ENABLED) {
                    //     depthbuffer_position = depthbuffer.getBufferPositionAt(x_start, y);
                    //     if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                    //         depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                    //         if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //             framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                    //         }
                    //     }
                    // }
                    // else if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //     framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                    // }
                    //
                    // for (int32 x = x_start; x < x_stop; ++x) {
                    //     shader_program.interpolateRight(&fragment, triangle);
                    //     if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //         framebuffer.moveBufferPositionRight(framebuffer_position);
                    //     }
                    //     if constexpr(shader_config.depth_test == DEPTH_TEST_ENABLED) {
                    //         depthbuffer.moveBufferPositionRight(depthbuffer_position);
                    //         if (fragment.inverse_depth < depthbuffer.readPixelAt(depthbuffer_position)) {
                    //             depthbuffer.drawPixelAt(depthbuffer_position, fragment.inverse_depth);
                    //             if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //                 framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                    //             }
                    //         }
                    //     }
                    //     else if constexpr(shader_config.shading == SHADING_ENABLED) {
                    //         framebuffer.drawPixelAt(framebuffer_position, shader_program.computeColor(&fragment, triangle));
                    //     }
                    // }
                }

                // Compute x-coords of edges at next scanline.
                rasterization.left_x += rasterization.left_dx_per_dy;
                rasterization.right_x += rasterization.right_dx_per_dy;
            }
        };

        if (rasterization.y_halftri_end < 0) {
            // Skip first half-triangle since it is fully outside screen.
            if (rasterization.y_fulltri_end < 0) {
                // Skip complete triangle since it is fully outside screen.
                return;
            }
            const int16 skip_num = rasterization.y_halftri_end - first_scanline_y + 1;
            rasterization.left_x += rasterization.left_dx_per_dy * static_cast<float>(skip_num);
            rasterization.right_x += rasterization.right_dx_per_dy * static_cast<float>(skip_num);
        }
        else {
            // Rasterize first half-triangle.
            rasterizeHalfTriangle(first_scanline_y);
            if (rasterization.y_halftri_end == rasterization.y_fulltri_end) {
                // No second half-triangle exists.
                return;
            }
        }

        // Adjust rasterization data for second half-triangle.
        const int16 next_y_start = rasterization.y_halftri_end + 1;
        rasterization.y_halftri_end = rasterization.y_fulltri_end;
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

        // Rasterize second half-triangle.
        rasterizeHalfTriangle(next_y_start);
    }

} // namespace MicroRenderer
