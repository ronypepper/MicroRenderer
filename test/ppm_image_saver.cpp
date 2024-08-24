//
// Created by pusdo on 18/07/2024.
//

#include "MicroRenderer/MicroRenderer.h"
#include "models/cube.h"
#include "models/rasterization_test.h"

using namespace MicroRenderer;

constexpr RendererConfiguration my_renderer_cfg = {SCANLINE, CLOCKWISE};
using MyRenderer = Renderer<float, my_renderer_cfg, UVCShaderProgram>;

int main(int argc, char *argv[])
{
    constexpr int32 window_width = 1000;//480;
    constexpr int32 window_height = 1000;//800;

    // Framebuffer and depthbuffer
    auto* framebuffer_memory = new uint8[window_width * window_height * 3];
    auto* depthbuffer_memory = new float[window_width * window_height];

    // Renderer creation.
    MyRenderer my_renderer;
    my_renderer.setResolution(window_width, window_height);
    my_renderer.setNearPlane(0.1f);

    // Update renderer's framebuffer and depthbuffer address.
    my_renderer.setFramebuffer(framebuffer_memory);
    my_renderer.setDepthbuffer(depthbuffer_memory);

    // Clear renderer's framebuffer and depthbuffer.
    my_renderer.getFramebuffer().setResolution(window_width, window_height);
    my_renderer.getFramebuffer().clearBuffer({0.f});
    my_renderer.getDepthbuffer().setResolution(window_width, window_height);
    my_renderer.getDepthbuffer().clearBuffer(std::numeric_limits<float>::max());

    // Rasterization test instance.
    //UVCInstanceData<float> rasterization_test_instance = {{1.f}};

    // Cube instance.
    constexpr uint16 num_render_instructions = 2;
    MyRenderer::RenderInstruction render_instructions[num_render_instructions] = {
        {0, {1.f, {test_texture_rgb888, 3, 3}}},
        {0, {1.f, {test_texture2_rgb888, 3, 3}}}
    };
    float angle = 0.f;
    float offset = 0.f;

    // Renderer setup.
    my_renderer.setGlobalData(nullptr);
    my_renderer.setModels(&cube_model);
    my_renderer.setRenderInstructions(render_instructions, num_render_instructions);

    UVCVertexBuffer<float> vertex_buffer[100];
    MyRenderer::RasterizationBuffer rasterization_buffers[200];
    MyRenderer::RasterizationOrder rasterization_order[200];
    my_renderer.setVertexBuffers(vertex_buffer);
    my_renderer.setRasterizationBuffers(rasterization_buffers, rasterization_order);

    // Camera state.
    Vector3<float> cam_position = {0.f, 0.f, -2.f};
    Vector3<float> cam_rotation = {0.f, 0.f, 0.f};

    // Camera directions.
    const Vector3<float> cam_view_dir = (Transform::rotationEuler(cam_rotation) * vec4(0.f, 0.f, 1.f, 1.f)).getXYZ();
    const Vector3<float> cam_up_dir = (Transform::rotationEuler(cam_rotation) * vec4(0.f, 1.f, 0.f, 1.f)).getXYZ();

    // Cube 1 transform.
    mat4 full_tf = Transform::viewport<float>(window_width, window_height, false, false);
    full_tf *= Transform::perspectiveProjection(-0.02f, 0.02f, -0.02f, 0.02f, 0.1f, 10.f);
    //full_tf *= Transform::perspectiveProjection(45.f, 1.f, 1.f, 5.f);
    //full_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    full_tf *= Transform::camera(cam_position, cam_view_dir, cam_up_dir);
    full_tf *= Transform::translation(vec3(-1.f, 0.f, 3.5f - offset));
    full_tf *= Transform::rotationEuler(vec3(0.f, 0.f, 0.f));
    full_tf *= Transform::scale(vec3(1.0f));
    render_instructions[0].instance_data.model_screen_tf = full_tf;

    // Cube 2 transform.
    full_tf = Transform::viewport<float>(window_width, window_height, false, false);
    full_tf *= Transform::perspectiveProjection(-0.02f, 0.02f, -0.02f, 0.02f, 0.1f, 10.f);
    //full_tf *= Transform::perspectiveProjection(45.f, 1.f, 1.f, 5.f);
    //full_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    full_tf *= Transform::camera(cam_position, cam_view_dir, cam_up_dir);
    full_tf *= Transform::translation(vec3(1.f, 0.f, 3.5f - offset));
    full_tf *= Transform::rotationEuler(vec3(0.f, 0.f, 0.f));
    full_tf *= Transform::scale(vec3(1.0f));
    render_instructions[1].instance_data.model_screen_tf = full_tf;

    // Render call.
    my_renderer.render();

    // Iterate over scanlines.
    for (int32 y = 0; y < window_height; ++y) {
        my_renderer.setFramebuffer(reinterpret_cast<MyRenderer::Framebuffer::InternalType*>(framebuffer_memory) + y * window_width);
        my_renderer.setDepthbuffer(reinterpret_cast<MyRenderer::Depthbuffer::InternalType*>(depthbuffer_memory) + y * window_width);
        my_renderer.renderNextScanline();
    }

    // Draw rasterization test.
    /*my_renderer.renderFramebuffer();
    for (uint32 i = 12; i < 23; ++i) {
        my_renderer.getFramebuffer().setCursor(static_cast<int32>(rasterization_test_vertex_source[i].model_position.x),
                                               static_cast<int32>(rasterization_test_vertex_source[i].model_position.y));
        my_renderer.getFramebuffer().drawPixelAtCursor({1.f, 0.f, 0.f});
    }
    my_renderer.getFramebuffer().setCursor(479, static_cast<int32>(rasterization_test_vertex_source[23].model_position.y));
    my_renderer.getFramebuffer().drawPixelAtCursor({1.f, 0.f, 0.f});
    Vector2<float> near_plane_intersections[6] = {
        {260, 380}, {177.5, 415.25}, {233.33, 166.67}, {256.67, 212.33}, {450, 225}, {450, 375}
    };
    for (auto & intersection : near_plane_intersections) {
        my_renderer.getFramebuffer().setCursor(static_cast<int32>(intersection.x), static_cast<int32>(intersection.y));
        my_renderer.getFramebuffer().drawPixelAtCursor({0.f, 1.f, 0.f});
    }*/

    // Save framebuffer and depthbuffer to image.
    my_renderer.setFramebuffer(reinterpret_cast<MyRenderer::Framebuffer::InternalType*>(framebuffer_memory));
    my_renderer.getFramebuffer().setResolution(window_width, window_height);
    my_renderer.getFramebuffer().saveToPPMImage("framebuffer");
    my_renderer.setDepthbuffer(reinterpret_cast<MyRenderer::Depthbuffer::InternalType*>(depthbuffer_memory));
    my_renderer.getDepthbuffer().setResolution(window_width, window_height);
    my_renderer.getDepthbuffer().saveToPPMImage("depthbuffer");

    delete[] framebuffer_memory;
    delete[] depthbuffer_memory;
}
