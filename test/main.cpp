//
// Created by pusdo on 18/07/2024.
//

#include "MicroRenderer/MicroRenderer.h"

using namespace MicroRenderer;

int main(int argc, char *argv[])
{
    int32 window_width = 800;
    int32 window_height = 800;
    auto* buffer = new uint8[window_width * window_height * 3];

    // Renderer creation.
    typedef RendererConfig<float, UVCPerspShaderProgram<float>> MyConfig;
    constexpr MyConfig my_config = {RGB888};
    Renderer<MyConfig, my_config> my_renderer(window_width, window_height, buffer);
    my_renderer.getFramebuffer().setResolution(window_width, window_height);
    my_renderer.setFramebuffer(buffer);
    my_renderer.getFramebuffer().clearBuffer();
    my_renderer.setFrontFace(COUNTERCLOCKWISE);
    my_renderer.getShaderProgram().setNearPlane(1.f);

    // Cube model.
    mat4 cube_tf = Transform::viewport<float>(window_width, window_height, false, false);
    cube_tf *= Transform::perspectiveProjection(60.f, 1.f, 1.f, 5.f);
    //cube_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    cube_tf *= Transform::camera(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 1.f), vec3(0.f, 1.f, 0.f));
    cube_tf *= Transform::translation(vec3(0.3f, 0.3f, 2.5f));
    cube_tf *= Transform::rotationEuler(vec3(45.f, 45.f, 45.f));
    cube_tf *= Transform::scale(vec3(1.0f));

    //cube_tf *= Transform::orthographicProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    // cube_tf *= Transform::camera(vec3(0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
    // cube_tf *= Transform::translation(vec3(0.25f, -1.25f, 0.5f));
    // cube_tf *= Transform::rotationEuler(vec3(0.f, 0.f, 45.f));
    // cube_tf *= Transform::scale(vec3(0.6f));
    UVCVertexSource<float> cube_vertex_source[8] = {
        {{0.5f, 0.5f, -0.5f}, {0.f, 0.f, 1.f}},
        {{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, 0.5f}, {0.f, 1.f, 1.f}},
        {{0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}},
        {{-0.5f, 0.5f, -0.5f}, {1.f, 0.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {1.f, 1.f, 0.f}},
        {{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}},
        {{-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}}
    };
    UVCTriangleSource<float> cube_triangle_source[12] = {
        {{0.7f, 0.2f, 0.5f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{1.f, 0.f, 1.f}},
        {{1.f, 1.f, 0.f}},
        {{1.f, 1.f, 1.f}},
        {{3.f, 6.f, 8.f}},
        {{0.f, 0.7f, 1.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 1.f}}
    };
    TriangleIndices cube_indices[12] = {
        4, 2, 0, 2, 7, 3, 6, 5, 7, 1, 7, 5, 0, 3, 1, 4, 1, 5, 4, 6, 2, 2, 6, 7, 6, 4, 5, 1, 3, 7, 0, 2, 3, 4, 0, 1
    };
    UVCShaderInterface<float>::ModelData cube_model = {8, 12, cube_vertex_source, cube_triangle_source, cube_indices};
    UVCInstanceData<float> cube_instance = {cube_tf};

    // Triangle test model.
    constexpr uint32 tritest_vertex_num = 84;
    UVCVertexSource<float> tritest_vertex_source[tritest_vertex_num] = {
        {{100.f, 50.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, -50.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{-100.f, 80.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{380.f, 750.f, 10.5f}, {0.f, 1.f, 0.f}},
        {{430.f, 850.f, 5.5f}, {0.f, 0.f, 1.f}},
        {{580.f, 720.f, 200.f}, {0.f, 1.f, 1.f}},
        {{50.f, 850.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 750.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{-100.f, 720.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{430.f, -50.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{380.f, 50.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{580.f, 80.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{300.f, 400.f, -2.f}, {0.f, 1.f, 1.f}},
        {{200.f, 350.f, 3.f}, {0.f, 1.f, 1.f}},
        {{170.f, 437.f, -1.f}, {0.f, 1.f, 1.f}},
        {{300.f, 600.f, -9.f}, {0.f, 1.f, 1.f}},
        {{200.f, 550.f, -10.f}, {0.f, 1.f, 1.f}},
        {{170.f, 637.f, -12.f}, {0.f, 1.f, 1.f}},
        {{300.f, 200.f, -1.f}, {0.f, 1.f, 1.f}},
        {{200.f, 150.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{170.f, 237.f, 2.f}, {0.f, 1.f, 1.f}},
        {{400.f, 200.f, 5.f}, {0.f, 1.f, 1.f}},
        {{400.f, 400.f, 5.f}, {0.f, 1.f, 1.f}},
        {{600.f, 300.f, -15.f}, {0.f, 1.f, 1.f}},
        {{50.f, 75.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 100.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 100.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 100.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 125.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 100.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 200.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 225.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 200.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 175.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 200.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 200.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{100.f, 325.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 300.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{75.f, 300.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{100.f, 300.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{100.f, 325.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{125.f, 300.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{100.f, 400.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{100.f, 425.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{125.f, 400.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{100.f, 425.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{100.f, 400.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{75.f, 400.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 500.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 525.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{75.f, 500.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{75.f, 500.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{50.f, 525.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{75.f, 525.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{175.f, 500.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{150.f, 525.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{175.f, 525.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{150.f, 500.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{150.f, 525.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{175.f, 500.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 625.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{25.f, 600.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{25.f, 625.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{50.f, 625.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{50.f, 600.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{25.f, 600.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{150.f, 625.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{150.f, 600.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{125.f, 600.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{150.f, 625.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{125.f, 600.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{125.f, 625.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{175.f, 50.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{125.f, 50.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{150.f, 75.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{200.f, 75.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{250.f, 75.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{225.f, 50.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{125.f, 100.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{125.f, 150.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{150.f, 125.f, 0.5f}, {0.f, 1.f, 1.f}},
        {{250.f, 150.f, 0.5f}, {0.f, 1.f, 0.f}},
        {{250.f, 100.f, 0.5f}, {0.f, 0.f, 1.f}},
        {{225.f, 125.f, 0.5f}, {0.f, 1.f, 1.f}}
    };
    constexpr uint32 tritest_triangle_num = 28;
    UVCTriangleSource<float> tritest_triangle_source[tritest_triangle_num] = {
        {{1.f, 0.f, 0.f}},
        {{0.f, 1.f, 0.f}},
        {{1.f, 1.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{1.f, 0.f, 1.f}},
        {{0.f, 1.f, 1.f}},
        {{0.9f, 0.1f, 0.6f}},
        {{0.8f, 0.2f, 0.7f}},
        {{1.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 0.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{1.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 0.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{1.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 0.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{1.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 0.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 0.f}}
    };
    TriangleIndices tritest_indices[tritest_triangle_num] = {};
    for (uint32 i = 0; i < tritest_triangle_num; ++i) {
        tritest_indices[i].vertex_1_idx = static_cast<uint16>(3 * i);
        tritest_indices[i].vertex_2_idx = static_cast<uint16>(3 * i + 1);
        tritest_indices[i].vertex_3_idx = static_cast<uint16>(3 * i + 2);
    }
    UVCShaderInterface<float>::ModelData tritest_model = {
        tritest_vertex_num, tritest_triangle_num, tritest_vertex_source, tritest_triangle_source, tritest_indices
    };
    UVCInstanceData<float> tritest_instance = {{1.f}};

    // Renderer setup.
    UVCVertexBuffer<float> vertex_buffer[100];
    UVCTriangleBuffer<float> triangle_buffer[200];
    UVCShaderInterface<float>::ModelData* models[1] = {&cube_model};
    UVCInstanceData<float>* instances[1] = {&cube_instance};

    my_renderer.setRenderInstances(1, models, instances);
    my_renderer.setShadingBuffers(vertex_buffer, triangle_buffer);
    my_renderer.setRasterizationBuffers(nullptr, nullptr);

    // Draw cube.
    my_renderer.renderFramebuffer();

    // Draw triangle clip test.
    /*my_renderer.renderFramebuffer();
    for (uint32 i = 12; i < 23; ++i) {
        my_renderer.getFramebuffer().setCursor(static_cast<int32>(tritest_vertex_source[i].model_position.x), static_cast<int32>(tritest_vertex_source[i].model_position.y));
        my_renderer.getFramebuffer().drawPixelAtCursor({1.f, 0.f, 0.f});
    }
    my_renderer.getFramebuffer().setCursor(479, static_cast<int32>(tritest_vertex_source[23].model_position.y));
    my_renderer.getFramebuffer().drawPixelAtCursor({1.f, 0.f, 0.f});
    Vector2<float> near_plane_intersections[6] = {
        {260, 380}, {177.5, 415.25}, {233.33, 166.67}, {256.67, 212.33}, {450, 225}, {450, 375}
    };
    for (auto & intersection : near_plane_intersections) {
        my_renderer.getFramebuffer().setCursor(static_cast<int32>(intersection.x), static_cast<int32>(intersection.y));
        my_renderer.getFramebuffer().drawPixelAtCursor({0.f, 1.f, 0.f});
    }*/

    // Save result to image.
    my_renderer.getFramebuffer().saveToPPMImage("cube_image");

    delete[] buffer;
}
