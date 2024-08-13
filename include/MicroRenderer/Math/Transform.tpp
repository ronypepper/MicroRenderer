//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Transform.h"
#include <cmath>
#include "Utility.h"

namespace MicroRenderer {

template<typename T>
Matrix4<T> Transform::translation(const Vector3<T> &position) {
    return Matrix4<T>{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        position.x, position.y, position.z, 1
    };
}

template<typename T>
Matrix4<T> Transform::rotationRadians(const Vector3<T> &rotation) {
    const T cos_x = std::cos(rotation.x);
    const T sin_x = std::sin(rotation.x);
    const T cos_y = std::cos(rotation.y);
    const T sin_y = std::sin(rotation.y);
    const T cos_z = std::cos(rotation.z);
    const T sin_z = std::sin(rotation.z);
    return Matrix4<T>{
        cos_y * cos_z, cos_y * sin_z, -sin_y, 0,
        sin_x * sin_y * cos_z - cos_x * sin_z, sin_x * sin_y * sin_z + cos_x * cos_z, sin_x * cos_y, 0,
        cos_x * sin_y * cos_z + sin_x * sin_z, cos_x * sin_y * sin_z - sin_x * cos_z, cos_x * cos_y, 0,
        0, 0, 0, 1
    };
}

template<typename T>
Matrix4<T> Transform::rotationEuler(const Vector3<T> &rotation) {
    return rotationRadians(degreeToRadians(rotation));
}

template<typename T>
Matrix4<T> Transform::scale(const Vector3<T> &scale) {
    return Matrix4<T>{
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    };
}

template<typename T>
Matrix4<T> Transform::camera(const Vector3<T> &eye, const Vector3<T> &view_direction, const Vector3<T> &view_up) {
    const Vector3<T> w = view_direction.getNormalized();
    const Vector3<T> u = view_up.cross(w).getNormalized();
    const Vector3<T> v = w.cross(u);
    return Matrix4<T>{
        u.x, v.x, w.x, 0,
        u.y, v.y, w.y, 0,
        u.z, v.z, w.z, 0,
        -eye.dot(u), -eye.dot(v), -eye.dot(w), 1
    };
}

template<typename T>
Matrix4<T> Transform::orthographicProjection(T left, T right, T bottom, T top, T near, T far) {
    // return Matrix4<T>{
    //     2 / (right - left), 0, 0, 0,
    //     0, 2 / (top - bottom), 0, 0,
    //     0, 0, 2 / (far - near), 0,
    //     (right + left) / (left - right), (top + bottom) / (bottom - top), (far + near) / (far - near), 1
    // };
    return Matrix4<T>{
        2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, 1 / (far - near), 0,
        (right + left) / (left - right), (top + bottom) / (bottom - top), near / (near - far), 1
    };
}

template<typename T>
Matrix4<T> Transform::perspectiveProjection(T vertical_fov, T aspect_ratio, T near, T far) {
    const T c = 1 / std::tan(vertical_fov / 2);
    return Matrix4<T>{
        c / aspect_ratio, 0, 0, 0,
        0, c, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, 2 * far * near / (near - far), 0
    };
}

template<typename T>
Matrix4<T> Transform::viewport(uint32 viewport_width, uint32 viewport_height, bool mirror_x, bool mirror_y) {
    const T cast_width = static_cast<T>(viewport_width);
    const T cast_height = static_cast<T>(viewport_height);
    const T factor_x = mirror_x ? static_cast<T>(-0.5) : static_cast<T>(0.5);
    const T factor_y = mirror_y ? static_cast<T>(-0.5) : static_cast<T>(0.5);
    return Matrix4<T>{
        cast_width * factor_x, 0, 0, 0,
        0, cast_height * factor_y, 0, 0,
        0, 0, 1, 0,
        (cast_width - static_cast<T>(1.0)) * static_cast<T>(0.5), (cast_height - static_cast<T>(1.0)) * static_cast<T>(0.5), 0, 1
    };
}

} // namespace MicroRenderer
