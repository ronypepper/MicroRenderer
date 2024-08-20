//
// Created by pusdo on 18/07/2024.
//

#include "MicroRenderer/MicroRenderer.h"
#include "models/cube.h"
#include "models/rasterization_test.h"

using namespace MicroRenderer;

int main(int argc, char *argv[])
{
    constexpr int32 window_width = 1000;//480;
    constexpr int32 window_height = 1000;//800;

    // Framebuffer and depthbuffer
    auto* framebuffer_memory = new uint8[window_width * window_height * 3];
    auto* depthbuffer_memory = new float[window_width * window_height];

    // Renderer creation.
    Renderer<float, UVCShaderProgram> my_renderer;
    my_renderer.setFrontFace(CLOCKWISE);
    my_renderer.setNearPlane(0.1f);
    my_renderer.setFramebuffer(framebuffer_memory, window_width, window_height);
    my_renderer.setDepthbuffer(depthbuffer_memory, window_width, window_height);

    // Clear renderer's framebuffer and depthbuffer.
    my_renderer.getFramebuffer().clearBuffer({0.f});
    my_renderer.getDepthbuffer().clearBuffer(std::numeric_limits<float>::max());

    // Rasterization test instance.
    //UVCInstanceData<float> rasterization_test_instance = {{1.f}};

    // Cube instance.
    UVCInstanceData<float> cube_instance_1 = {{1.f}};
    UVCInstanceData<float> cube_instance_2 = {{1.f}};
    float angle = 0.f;
    float offset = 0.f;

    // Renderer setup.
    UVCVertexBuffer<float> vertex_buffer[100];
    UVCTriangleBuffer<float> triangle_buffer[200];
    UVCShaderInterface<float>::ModelData* models[2] = {&cube_model, &cube_model};
    UVCInstanceData<float>* instances[2] = {&cube_instance_1, &cube_instance_2};
    my_renderer.getShaderProgram().setGlobalData(&cube_global_data);
    my_renderer.setRenderInstances(2, models, instances);
    my_renderer.setVertexBuffers(vertex_buffer);
    my_renderer.setScanlineBuffers(nullptr, nullptr);

    // Camera state.
    Vector3<float> cam_translation = {0.f, 0.f, 0.f};
    Vector3<float> cam_rotation = {0.f, 0.f, 0.f};

    // Camera directions.
    const Vector3<float> cam_view_dir = (Transform::rotationEuler(cam_rotation) * vec4(0.f, 0.f, 1.f, 1.f)).getXYZ();
    const Vector3<float> cam_up_dir = (Transform::rotationEuler(cam_rotation) * vec4(0.f, 1.f, 0.f, 1.f)).getXYZ();

    // Cube 1 transform.
    mat4 full_tf = Transform::viewport<float>(window_width, window_height, false, false);
    full_tf *= Transform::perspectiveProjection(-0.02f, 0.02f, -0.02f, 0.02f, 0.1f, 10.f);
    //full_tf *= Transform::perspectiveProjection(45.f, 1.f, 1.f, 5.f);
    //full_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    full_tf *= Transform::camera(cam_translation, cam_view_dir, cam_up_dir);
    full_tf *= Transform::translation(vec3(-1.f, 0.f, 3.5f - offset));
    full_tf *= Transform::rotationEuler(vec3(0.f, 0.f, 0.f));
    full_tf *= Transform::scale(vec3(1.0f));
    cube_instance_1.model_screen_tf = full_tf;

    // Cube 2 transform.
    full_tf = Transform::viewport<float>(window_width, window_height, false, false);
    full_tf *= Transform::perspectiveProjection(-0.02f, 0.02f, -0.02f, 0.02f, 0.1f, 10.f);
    //full_tf *= Transform::perspectiveProjection(45.f, 1.f, 1.f, 5.f);
    //full_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    full_tf *= Transform::camera(cam_translation, cam_view_dir, cam_up_dir);
    full_tf *= Transform::translation(vec3(1.f, 0.f, 3.5f - offset));
    full_tf *= Transform::rotationEuler(vec3(0.f, 0.f, 0.f));
    full_tf *= Transform::scale(vec3(1.0f));
    cube_instance_2.model_screen_tf = full_tf;

    // Render call.
    my_renderer.renderFramebuffer();

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
    my_renderer.getFramebuffer().saveToPPMImage("framebuffer");
    my_renderer.getDepthbuffer().saveToPPMImage("depthbuffer");

    delete[] framebuffer_memory;
    delete[] depthbuffer_memory;
}
