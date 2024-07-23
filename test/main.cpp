//
// Created by pusdo on 18/07/2024.
//

#include "MicroRenderer/MicroRenderer.h"

using namespace MicroRenderer;

int main(int argc, char *argv[])
{
    uint32 window_width = 480;
    uint32 window_height = 800;
    mat4 transform = Transform::viewport<float>(window_width, window_height);
    transform *= Transform::orthogonalProjection(-2.f, 2.f, -2.f, 2.f, -2.f, 2.f);
    transform *= Transform::camera(vec3(0.f), vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
    transform *= Transform::translation(vec3(0.f, 2.f, 0.f));
    transform *= Transform::rotationEuler(vec3(0.f));
    transform *= Transform::scale(vec3(1.f));

    auto* buffer = new uint8[window_width * window_height];

    Renderer<float, RGB888> Renderer(window_width, window_height, buffer);
    Vector3<float> v1(30, 30, 0);
    Vector3<float> v2(200, 300, 0);
    Vector3<float> v3(250, 100, 0);
    Renderer.rasterizeTriangle<BaseShader>(&v1, &v2, &v3);

    delete[] buffer;
}
