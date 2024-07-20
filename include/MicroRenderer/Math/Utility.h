//
// Created by pusdo on 18/07/2024.
//

#pragma once
#include <cmath>

namespace MicroRenderer {
    template<typename T>
    T radiansToDegrees(T value) {
        return value * static_cast<T>(180.0 / M_PI);
    }

    template<typename T>
    T degreeToRadians(T value) {
        return value * static_cast<T>(M_PI / 180.0);
    }
} // namespace MicroRenderer
