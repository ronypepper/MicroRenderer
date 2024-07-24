//
// Created by pusdo on 22/07/2024.
//

#pragma once
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T>
class BaseShader {
public:
    bool initializeShading(const Vector3<T>* v1, const Vector3<T>* v2, const Vector3<T>* v3) { return true; }
    void setPosition(uint32 x, uint32 y) {};
    void movePositionRight() {}
    Vector3<T> computeShading() { return {static_cast<T>(1.0)}; }
};

}// namespace MicroRenderer
