//
// Created by pusdo on 18/07/2024.
//

#include "MicroRenderer/MicroRenderer.h"

using namespace MicroRenderer;

int main(int argc, char *argv[])
{
    int32 window_width = 480;
    int32 window_height = 800;
    mat4 transform = Transform::viewport<float>(window_width, window_height);
    transform *= Transform::orthogonalProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    transform *= Transform::camera(vec3(0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
    transform *= Transform::translation(vec3(0.f, 2.f, 0.f));
    transform *= Transform::rotationEuler(vec3(0.f));
    transform *= Transform::scale(vec3(1.f));

    auto* buffer = new uint8[window_width * window_height * 3];

    // Renderer creation.
    typedef RendererConfig<float, UVCOrthoShaderProgram<float>> MyConfig;
    constexpr MyConfig my_config = {RGB888};
    Renderer<MyConfig, my_config> my_renderer(window_width, window_height, buffer);
    my_renderer.getFramebuffer().setResolution(window_width, window_height);
    my_renderer.setFramebuffer(buffer);
    my_renderer.getFramebuffer().clearBuffer();

    // Cube model.
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
        {{0.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 1.f}},
        {{1.f, 0.f, 0.f}},
        {{1.f, 0.f, 1.f}},
        {{1.f, 1.f, 0.f}},
        {{1.f, 1.f, 1.f}},
        {{0.f, 0.f, 0.f}},
        {{0.f, 0.f, 1.f}},
        {{0.f, 1.f, 0.f}},
        {{0.f, 1.f, 1.f}}
    };
    TriangleIndices cube_indices[12] = {
        5, 3, 1, 3, 8, 4, 7, 6, 8, 2, 8, 6, 1, 4, 2, 5, 2, 6, 5, 7, 3, 3, 7, 8, 7, 5, 6, 2, 4, 8, 1, 3, 4, 5, 1, 2
    };
    UVCShaderInterface<float>::ModelData cube_model = {8, 12, cube_vertex_source, cube_triangle_source, cube_indices};
    UVCInstanceData<float> cube_instance = {transform};

    // Triangle test model.
    constexpr uint32 tritest_vertex_num = 48;
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
        {{75.f, 400.f, 0.5f}, {0.f, 1.f, 1.f}}
    };
    constexpr uint32 tritest_triangle_num = 16;
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
    };
    TriangleIndices tritest_indices[tritest_triangle_num] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
    };
    UVCShaderInterface<float>::ModelData tritest_model = {
        tritest_vertex_num, tritest_triangle_num, tritest_vertex_source, tritest_triangle_source, tritest_indices
    };
    UVCInstanceData<float> tritest_instance = {{1.f}};

    // Renderer setup.
    UVCVertexBuffer<float> vertex_buffer[100];
    UVCTriangleBuffer<float> triangle_buffer[200];
    UVCShaderInterface<float>::ModelData* models[1] = {&tritest_model};
    UVCInstanceData<float>* instances[1] = {&tritest_instance};

    my_renderer.setRenderInstances(1, models, instances);
    my_renderer.setShadingBuffers(vertex_buffer, triangle_buffer);
    my_renderer.setRasterizationBuffers(nullptr, nullptr);

    // Draw triangle clip test.
    my_renderer.renderFramebuffer();
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
    }

    // Save result to image.
    my_renderer.getFramebuffer().saveToPPMImage("test_image");

    delete[] buffer;
}
