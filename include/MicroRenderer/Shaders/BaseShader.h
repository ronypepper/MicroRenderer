//
// Created by pusdo on 22/07/2024.
//

#pragma once
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T>
class BaseShader {
public:
    void initializeShading(const Vector3<T>* v1, const Vector3<T>* v2, const Vector3<T>* v3, uint32 x, uint32 y) {}
    void advanceShadingInX() {}
    void advanceShadingInXBy(uint32 delta_x) {}
    void advanceShadingInY() {}
    Vector3<T> computeShading() { return {static_cast<T>(1.0)}; }
};

}// namespace MicroRenderer
