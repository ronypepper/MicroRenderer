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
    template<typename CfgType, CfgType Config>
    Renderer<CfgType, Config>::Renderer(uint32 width, uint32 height, uint8* frame_buffer) : framebuffer(width, height, frame_buffer) {}

    template<typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::setFramebuffer(uint8* address) {
        framebuffer.setAddress(address);
    }

    template<typename CfgType, CfgType Config>
    typename Renderer<CfgType, Config>::RendererFramebuffer& Renderer<CfgType, Config>::getFramebuffer() {
        return framebuffer;
    }

    template<typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::setDepthbuffer(float* address) {
        depthbuffer = address;
    }

    template<typename CfgType, CfgType Config>
    float* Renderer<CfgType, Config>::getDepthbuffer() const {
        return depthbuffer;
    }

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::setRenderInstances(uint16 number, const ModelData* const* models,
                                                       const InstanceData* const* instances)
    {
        num_instances = number;
        instance_models = models;
        instance_datas = instances;
    }

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::setShadingBuffers(VertexBuffer* vertex, TriangleBuffer* triangle)
    {
        vertex_buffers = vertex;
        triangle_buffers = triangle;
    }

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::setRasterizationBuffers(RasterizationData<T>* data, RasterizationOrder* order)
    {
        rasterization_data_buffer = data;
        rasterization_order_buffer = order;
    }

    template<typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::rasterizeLineDDASafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
    {
        x0 = std::clamp(x0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y0 = std::clamp(y0, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        x1 = std::clamp(x1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getWidth()) - static_cast<T>(0.5));
        y1 = std::clamp(y1, static_cast<T>(-0.5), static_cast<T>(framebuffer.getHeight()) - static_cast<T>(0.5));
        rasterizeLineDDAUnsafe(x0, y0, x1, y1, color);
    }

    template<typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::rasterizeLineDDAUnsafe(T x0, T y0, T x1, T y1, const Vector3<T>& color)
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

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::rasterizeTriangle(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c)
    {
        RasterizationData<T> rasterization;
        int16 y_start = -1;
        if (setupTriangleRasterization({a, 0}, {b, 0}, {c, 0}, rasterization, y_start)) {
            rasterizeAndShadeTriangle(rasterization, {}, y_start);
        }
    }

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::renderFramebuffer()
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
            for (uint16 vertex_idx = 0; vertex_idx < model->num_vertices; ++vertex_idx) {
                const VertexData vertex_data = {model->vertices + vertex_idx, vertex_buffers + vertex_idx};

                // Transform vertex.
                shader_program.transformVertex(vertex_data);

                // Shade vertex.
                shader_program.shadeVertex(vertex_data);

                // Homogenize vertex.
                if constexpr(ShaderProgram::getProjectionType() == PERSPECTIVE) {
                    Vector4<T>& homogenous_pos = shader_program.getHomogenousSpacePosition(vertex_data);
                    if (homogenous_pos.w >= shader_program.getNearPlane()) {
                        homogenous_pos.w = static_cast<T>(1.0) / homogenous_pos.w;
                        Vector3<T>& screen_pos = shader_program.getScreenSpacePosition(vertex_data);
                        screen_pos.x = homogenous_pos.x * homogenous_pos.w;
                        screen_pos.y = homogenous_pos.y * homogenous_pos.w;
                        screen_pos.z = homogenous_pos.z * homogenous_pos.w;
                    }
                }
            }

            // Triangle processing.
            for (uint16 tri_idx = 0; tri_idx < model->num_triangles; ++tri_idx) {
                // Lambda for checking if vertex is outside clip region.
                auto isVertexOutsideClipRegion = [&, this](const Vector3<T>& position) -> bool {
                    if (position.x <= left_x_clip || position.x >= right_x_clip || position.y <= bottom_y_clip ||
                        position.y >= top_y_clip) {
                        return true;
                    }
                    return false;
                };

                // Gather vertex data.
                const uint16 v1_idx = model->indices[tri_idx].vertex_1_idx;
                const uint16 v2_idx = model->indices[tri_idx].vertex_2_idx;
                const uint16 v3_idx = model->indices[tri_idx].vertex_3_idx;
                const VertexData vertices[3] = {
                    {model->vertices + v1_idx, vertex_buffers + v1_idx},
                    {model->vertices + v2_idx, vertex_buffers + v2_idx},
                    {model->vertices + v3_idx, vertex_buffers + v3_idx}
                };
                const Vector3<T>& v1_screen_pos = shader_program.getScreenSpacePosition(vertices[0]);
                const Vector3<T>& v2_screen_pos = shader_program.getScreenSpacePosition(vertices[1]);
                const Vector3<T>& v3_screen_pos = shader_program.getScreenSpacePosition(vertices[2]);

                // Skip triangle if not visible.
                const bool v1_xy_invisible = isVertexOutsideClipRegion(v1_screen_pos);
                const bool v2_xy_invisible = isVertexOutsideClipRegion(v2_screen_pos);
                const bool v3_xy_invisible = isVertexOutsideClipRegion(v3_screen_pos);
                if (v1_xy_invisible && v2_xy_invisible && v3_xy_invisible) {
                    continue;
                }

                // Check if triangle is visible in z and clipped.
                const bool is_persp = ShaderProgram::getProjectionType() == PERSPECTIVE;
                const T v1_clip_depth = is_persp ? shader_program.getHomogenousSpacePosition(vertices[0]).z : v1_screen_pos.z;
                const T v2_clip_depth = is_persp ? shader_program.getHomogenousSpacePosition(vertices[1]).z : v2_screen_pos.z;
                const T v3_clip_depth = is_persp ? shader_program.getHomogenousSpacePosition(vertices[2]).z : v3_screen_pos.z;
                const uint8 v1_z_visible = v1_clip_depth > static_cast<T>(0.0);
                const uint8 v2_z_visible = v2_clip_depth > static_cast<T>(0.0);
                const uint8 v3_z_visible = v3_clip_depth > static_cast<T>(0.0);
                const uint8 num_visible_verts = v1_z_visible + v2_z_visible + v3_z_visible;
                if (num_visible_verts) {
                    // Triangle is at least partially visible. Rasterize/shade accordingly.

                    // Temporary triangle rasterization/shading data.
                    TriangleBuffer tri_buffer;
                    TriangleData triangle = {model->triangles + tri_idx, &tri_buffer};
                    RasterizationData<T> rasterization;
                    int16 y_start;

                    if (num_visible_verts == 3) {
                        // Triangle fully visible (in z).
                        shader_program.setupTriangle(vertices[0], vertices[1], vertices[2], triangle);
                        if (setupTriangleRasterization(vertices[0], vertices[1], vertices[2], rasterization, y_start)) {
                            rasterizeAndShadeTriangle(rasterization, triangle, y_start);
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

                            shader_program.setupTriangle(vertices[idx_3], clipped_vertices[1], clipped_vertices[0], triangle);
                            if (setupTriangleRasterization(vertices[idx_3], clipped_vertices[1], clipped_vertices[0], rasterization, y_start)) {
                                rasterizeAndShadeTriangle(rasterization, triangle, y_start);
                            }
                            shader_program.setupTriangle(vertices[idx_3], clipped_vertices[0], vertices[idx_2], triangle);
                            if (setupTriangleRasterization(vertices[idx_3], clipped_vertices[0], vertices[idx_2], rasterization, y_start)) {
                                rasterizeAndShadeTriangle(rasterization, triangle, y_start);
                            }
                        }
                        else {
                            // Triangle is clipped into one new triangle.
                            const uint8 idx_1 = v1_z_visible ? 0 : v2_z_visible ? 1 : 2;
                            const uint8 idx_2 = idx_1 == 2 ? 0 : idx_1 + 1;
                            const uint8 idx_3 = idx_2 == 2 ? 0 : idx_2 + 1;

                            shader_program.interpolateVertices(vertices[idx_2], vertices[idx_1], clipped_sources, clipped_buffers);
                            shader_program.interpolateVertices(vertices[idx_3], vertices[idx_1], clipped_sources + 1, clipped_buffers + 1);

                            shader_program.setupTriangle(vertices[idx_1], clipped_vertices[0], clipped_vertices[1], triangle);
                            if (setupTriangleRasterization(vertices[idx_1], clipped_vertices[0], clipped_vertices[1], rasterization, y_start)) {
                                rasterizeAndShadeTriangle(rasterization, triangle, y_start);
                            }
                        }
                    }
                }
            }
        }
    }

    template <typename CfgType, CfgType Config>
    bool Renderer<CfgType, Config>::setupTriangleRasterization(const VertexData& vertex_1, const VertexData& vertex_2,
        const VertexData& vertex_3, RasterizationData<T>& rasterization, int16& y_start)
    {
        const Vector3<T>& v1_screen_pos = shader_program.getScreenSpacePosition(vertex_1);
        const Vector3<T>& v2_screen_pos = shader_program.getScreenSpacePosition(vertex_2);
        const Vector3<T>& v3_screen_pos = shader_program.getScreenSpacePosition(vertex_3);
        return setupTriangleRasterization(v1_screen_pos, v2_screen_pos, v3_screen_pos, rasterization, y_start);
    }

    template <typename CfgType, CfgType Config>
    bool Renderer<CfgType, Config>::setupTriangleRasterization(const Vector3<T>& v1_screen_pos,
                                                               const Vector3<T>& v2_screen_pos,
                                                               const Vector3<T>& v3_screen_pos,
                                                               RasterizationData<T>& rasterization, int16& y_start)
    {

        // Lambda for setting up a triangle for rasterization.
        auto orderedSetup = [&rasterization, &y_start](const Vector3<T>& start, const Vector3<T>& left,
                                                       const Vector3<T>& right, const Vector2<T>& start_to_left,
                                                       const Vector2<T>& start_to_right,
                                                       const Vector2<T>& left_to_right) -> bool
        {
            // Triangle is rasterized separately for upper and lower half-triangle
            // (separated at scanline through middle vertex)

            // Determine if there are one or two half triangles and compute their edge slopes and edge starts.
            // Checks against division by zero are performed.
            constexpr T MIN_DELTA_Y = static_cast<T>(0.001);
            const bool start_right_too_close = start_to_right.y < MIN_DELTA_Y;
            const bool start_left_too_close = start_to_left.y < MIN_DELTA_Y;
            const bool left_right_too_close = std::abs(left_to_right.y) < MIN_DELTA_Y;
            if (start_right_too_close || start_left_too_close) {
                if (left_right_too_close) {
                    // No triangle at all.
                    return false;
                }
                if (start_right_too_close) {
                    // Only one half-triangle with flat start, pointing left or up.
                    rasterization.y_halftri_end = static_cast<int16>(std::ceil(left.y));
                    rasterization.left_dx_per_dy = start_to_left.x / start_to_left.y;
                    rasterization.right_x = right.x;
                }
                else {
                    // Only one half-triangle with flat start, pointing right.
                    rasterization.y_halftri_end = static_cast<int16>(std::ceil(right.y));
                    rasterization.left_dx_per_dy = start_to_right.x / start_to_right.y;
                    rasterization.right_x = left.x;
                }
                rasterization.y_fulltri_end = rasterization.y_halftri_end;
                rasterization.left_x = start.x;
                rasterization.right_dx_per_dy = left_to_right.x / left_to_right.y;

                // Compute depth gradient.
                const T left_to_right_z = right.z - left.z;
                rasterization.diz_per_dx = left_to_right_z / left_to_right.x;
                rasterization.diz_per_dy = left_to_right_z / left_to_right.y;
            }
            else {
                rasterization.left_dx_per_dy = start_to_left.x / start_to_left.y;
                rasterization.right_dx_per_dy = start_to_right.x / start_to_right.y;
                rasterization.left_x = start.x;
                rasterization.right_x = start.x;

                if (left_right_too_close) {
                    // Only one half-triangle with flat end.
                    rasterization.y_halftri_end = static_cast<int16>(std::ceil(std::min(left.y, right.y)));
                    rasterization.y_fulltri_end = rasterization.y_halftri_end;
                }
                else {
                    // Two half triangles.
                    rasterization.last_dx_per_dy = left_to_right.x / left_to_right.y;
                    if (left_to_right.y > static_cast<T>(0.0)) {
                        // Left is middle.
                        const T scanline_middle = std::ceil(left.y); // First scanline of lower half triangle.
                        rasterization.y_halftri_end = static_cast<int16>(scanline_middle);
                        rasterization.y_fulltri_end = static_cast<int16>(std::ceil(right.y));
                        rasterization.last_x = left.x + rasterization.last_dx_per_dy * (scanline_middle - left.y);
                    }
                    else {
                        // Right is middle.
                        rasterization.last_dx_per_dy = left_to_right.x / left_to_right.y;
                        const T scanline_middle = std::ceil(right.y); // First scanline of lower half triangle.
                        rasterization.y_halftri_end = static_cast<int16>(scanline_middle);
                        rasterization.y_fulltri_end = static_cast<int16>(std::ceil(left.y));
                        rasterization.last_x = right.x + rasterization.last_dx_per_dy * (scanline_middle - right.y);
                    }
                }
            }

            // Compute first scanline and its offset from start vertex.
            const T scanline_start = std::ceil(start.y);
            y_start = static_cast<int16>(scanline_start);
            const T y_start_offset = scanline_start - start.y;

            // Adjust edge starts to start scanline.
            rasterization.left_x += rasterization.left_dx_per_dy * y_start_offset;
            rasterization.right_x += rasterization.right_dx_per_dy * y_start_offset;

            // Compute depth gradient and start depth on first scanline.
            const T start_to_left_z = left.z - start.z;
            rasterization.diz_per_dx = start_to_left_z / start_to_left.x;
            rasterization.diz_per_dy = start_to_left_z / start_to_left.y;
            rasterization.left_inverse_z = start.z + rasterization.diz_per_dy * y_start_offset;

            return true;
        };

        // Compute triangle edge deltas.
        const Vector2<T> v1_to_v2 = {v2_screen_pos.x - v1_screen_pos.x, v2_screen_pos.y - v1_screen_pos.y};
        const Vector2<T> v1_to_v3 = {v3_screen_pos.x - v1_screen_pos.x, v3_screen_pos.y - v1_screen_pos.y};
        const Vector2<T> v2_to_v3 = {v3_screen_pos.x - v2_screen_pos.x, v3_screen_pos.y - v2_screen_pos.y};

        // Backface culling and degenerate check.
        constexpr T DEGENERATE_THRESHOLD = static_cast<T>(0.001);
        const T area = v1_to_v2.x * v1_to_v3.y - v1_to_v2.y * v1_to_v3.x;
        if (area > DEGENERATE_THRESHOLD)
            return false;

        // Determine vertex order and perform rasterization setup.
        if (v1_to_v2.y >= static_cast<T>(0.0)) {
            // Vertex 2 is NOT bottommost.
            if (v1_to_v3.y >= static_cast<T>(0.0)) {
                // Vertex 1 is bottommost.
                if (v2_to_v3.x >= static_cast<T>(0.0)) {
                    // Vertex 2 is left of Vertex 3.
                    return orderedSetup(v1_screen_pos, v2_screen_pos, v3_screen_pos, v1_to_v2, v1_to_v3, v2_to_v3);
                }
                // Vertex 3 is left of Vertex 2.
                return orderedSetup(v1_screen_pos, v3_screen_pos, v2_screen_pos, v1_to_v3, v1_to_v2, -v2_to_v3);
            }
            // Vertex 3 is bottommost.
            if (v1_to_v2.x >= static_cast<T>(0.0)) {
                // Vertex 1 is left of Vertex 2.
                return orderedSetup(v3_screen_pos, v1_screen_pos, v2_screen_pos, -v1_to_v3, -v2_to_v3, v1_to_v2);
            }
            // Vertex 2 is left of Vertex 1.
            return orderedSetup(v3_screen_pos, v2_screen_pos, v1_screen_pos, -v2_to_v3, -v1_to_v3, -v1_to_v2);
        }
        // Vertex 1 is NOT bottommost.
        if (v2_to_v3.y >= static_cast<T>(0.0)) {
            // Vertex 2 is bottommost.
            if (v1_to_v3.x >= static_cast<T>(0.0)) {
                // Vertex 1 is left of Vertex 3.
                return orderedSetup(v2_screen_pos, v1_screen_pos, v3_screen_pos, -v1_to_v2, v2_to_v3, v1_to_v3);
            }
            // Vertex 3 is left of Vertex 1.
            return orderedSetup(v2_screen_pos, v3_screen_pos, v1_screen_pos, v2_to_v3, -v1_to_v2, -v1_to_v3);
        }
        // Vertex 3 is bottommost.
        if (v1_to_v2.x >= static_cast<T>(0.0)) {
            // Vertex 1 is left of Vertex 2.
            return orderedSetup(v3_screen_pos, v1_screen_pos, v2_screen_pos, -v1_to_v3, -v2_to_v3, v1_to_v2);
        }
        // Vertex 2 is left of Vertex 1.
        return orderedSetup(v3_screen_pos, v2_screen_pos, v1_screen_pos, -v2_to_v3, -v1_to_v3, -v1_to_v2);
    }

    template <typename CfgType, CfgType Config>
    void Renderer<CfgType, Config>::rasterizeAndShadeTriangle(RasterizationData<T>& rasterization,
                                                              const TriangleData& triangle, int16 first_scanline_y)
    {
        // Triangle is split into upper and lower half-triangle, which are rasterized/shaded separately.
        // Upper half: scanlines contained between top-most two edges.
        // Lower half: scanlines contained between bottom-most two edges.

        // Define lamba that implements the "top-left" rule for rasterizing edges going exactly through pixel points.
        auto isTopOrLeftEdge = [](const T delta_x, const T delta_y) -> bool {
            const bool isLeftEdge = delta_y > 0;
            const bool isTopEdge = delta_y == 0 && delta_x < 0;
            return isLeftEdge || isTopEdge;
        };

        // // Adjust edge x-coords according to "top-left" rule.
        // T x_start_left_edge = v1->x;
        // if (dy_top_middle == static_cast<T>(0.0)) {
        //     // v1-v2 is top edge and v1-v3 is left edge.
        //     v1->x += std::numeric_limits<T>::min();
        //     v3->x += std::numeric_limits<T>::min();
        // }
        // else {
        //     if (dx_middle_bottom > static_cast<T>(0.0)) {
        //         // v1-v2 is left edge.
        //         v1->x += std::numeric_limits<T>::min();
        //         v2->x += std::numeric_limits<T>::min();
        //         if (dy_middle_bottom > static_cast<T>(0.0)) {
        //             // v2-v3 is left edge.
        //             v1->x += std::numeric_limits<T>::min();
        //         }
        //     }
        //     else {
        //         // v1-v3 is left edge.
        //     }
        // }

        // Lambda for rasterizing half a triangle.
        auto rasterizeHalfTriangle = [&rasterization, &triangle, this](int16 y_start) {
            const int32 clipped_y_start = std::max(static_cast<int32>(y_start), static_cast<int32>(0));
            for (auto y = static_cast<int32>(y_start); y < clipped_y_start; ++y) {
                // Skip lines above screen.
                rasterization.left_x += rasterization.left_dx_per_dy;
                rasterization.right_x += rasterization.right_dx_per_dy;
            }
            const int32 clipped_y_end = std::min(static_cast<int32>(rasterization.y_halftri_end), framebuffer.getHeight());
            for (int32 y = clipped_y_start; y < clipped_y_end; ++y) {
                // Compute start and end of line.
                const int32 x_start = std::max(static_cast<int32>(std::ceil(rasterization.left_x)), static_cast<int32>(0));
                const int32 x_stop = std::min(static_cast<int32>(std::floor(rasterization.right_x)), framebuffer.getWidth() - 1);

                // Advance framebuffer and shader to start of next line.
                framebuffer.setCursor(x_start, y);
                shader_program.interpolateTo(triangle, x_start, y);

                // Shade and draw rasterized line.
                for (int32 x = x_start; x <= x_stop; ++x) {
                    framebuffer.drawPixelAtCursor(shader_program.shadeFragment(triangle));
                    framebuffer.moveCursorRight();
                    shader_program.interpolateRight(triangle);
                }

                // Compute x-coords of line at next scanline.
                rasterization.left_x += rasterization.left_dx_per_dy;
                rasterization.right_x += rasterization.right_dx_per_dy;
            }
        };

        // Lambda for adjusting rasterization data for lower triangle and signaling triangle end.
        auto updateRasterization = [&rasterization]() -> bool
        {
            if (rasterization.y_halftri_end == rasterization.y_fulltri_end)
                return false;
            rasterization.y_halftri_end = rasterization.y_fulltri_end;
            if (rasterization.last_dx_per_dy > static_cast<T>(0.0)) {
                // Left is middle.
                rasterization.left_x = rasterization.last_x;
                rasterization.left_dx_per_dy = rasterization.last_dx_per_dy;
            }
            else {
                // Right is middle.
                rasterization.right_x = rasterization.last_x;
                rasterization.right_dx_per_dy = rasterization.last_dx_per_dy;
            }
            return true;
        };

        // Rasterize first half triangle.
        rasterizeHalfTriangle(first_scanline_y);
        const int16 next_y_start = rasterization.y_halftri_end;

        // Update rasterization data for lower half triangle.
        if (!updateRasterization())
            return;

        // Rasterize second half triangle.
        rasterizeHalfTriangle(next_y_start);
    }

} // namespace MicroRenderer
