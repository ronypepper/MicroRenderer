//
// Created by pusdo on 29/06/2024.
//

#pragma once
#include "Framebuffer.h"
#include "MicroRenderer/Math/ScalarTypes.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, ColorCoding color_coding>
class Renderer {
public:

    Renderer(uint32 width, uint32 height, uint8* buffer);

    void rasterizeLineDDASafe(T x0, T x1, T y0, T y1, const Vector3<T> &color);

    void rasterizeLineDDAUnsafe(T x0, T x1, T y0, T y1, const Vector3<T> &color);

    Framebuffer<T, color_coding> framebuffer;
};

} // namespace MicroRenderer

#include "Renderer.tpp"
