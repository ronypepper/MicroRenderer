//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Transform.h"
#include <cmath>

namespace MicroRenderer {

template<typename T>
Matrix4<T> Transform::translation(const Vector3<T> &position) {
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        position.x, position.y, position.z, 1
    };
}

template<typename T>
Matrix4<T> Transform::rotation(const Vector3<T> &rotation) {
    T cos_x = std::cos(rotation.x);
    T sin_x = std::sin(rotation.x);
    T cos_y = std::cos(rotation.y);
    T sin_y = std::sin(rotation.y);
    T cos_z = std::cos(rotation.z);
    T sin_z = std::sin(rotation.z);
    return {
        cos_y * cos_z, cos_y * sin_z, -sin_y, 0,
        sin_x * sin_y * cos_z - cos_x * sin_z, sin_x * sin_y * sin_z + cos_x * cos_z, sin_x * cos_y, 0,
        cos_x * sin_y * cos_z + sin_x * sin_z, cos_x * sin_y * sin_z - sin_x * cos_z, cos_x * cos_y, 0,
        0, 0, 0, 1
    };
}

template<typename T>
Matrix4<T> Transform::scale(const Vector3<T> &scale) {
    return {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    };
}

template<typename T>
Matrix4<T> Transform::camera(const Vector3<T> &eye, const Vector3<T> &view_direction, const Vector3<T> &view_up) {
    const Vector3<T> w = -view_direction.getNormalized();
    const Vector3<T> u = view_up.cross(w).getNormalized();
    const Vector3<T> v = w.cross(u);
    return {
        u.x, v.x, w.x, 0,
        u.y, v.y, w.y, 0,
        u.z, v.z, w.z, 0,
        -eye.dot(u), -eye.dot(v), -eye.dot(w), 1
    };
}

template<typename T>
Matrix4<T> Transform::orthogonalProjection(T left, T right, T bottom, T top, T near, T far) {
    return {
        2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, 2 / (far - near), 0,
        (right + left) / (left - right), (top + bottom) / (bottom - top), (far + near) / (near - far), 1
    };
}

template<typename T>
Matrix4<T> Transform::perspectiveProjection(T vertical_fov, T aspect_ratio, T near, T far) {
    T c = 1 / std::tan(vertical_fov / 2);
    return {
        c / aspect_ratio, 0, 0, 0,
        0, c, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, 2 * far * near / (near - far), 0
    };
}

template<typename T>
Matrix4<T> Transform::viewport(int32 viewport_width, int32 viewport_height) {
    return {
        viewport_width / 2, 0, 0, 0,
        0, viewport_height / 2, 0, 0,
        0, 0, 1, 0,
        (viewport_width - 1) / 2, (viewport_height - 1) / 2, 0, 1
    };
}

} // namespace MicroRenderer
