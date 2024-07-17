//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Matrix4.h"
#include "Vector3.h"

namespace MicroRenderer {

class Transform {
public:
    template<typename T>
    static Matrix4<T> translation(const Vector3<T>& position);

    // Rotation around x, than y, than z: R(x,y,z) = R_z(z) * R_y(y) * R_x(x)
    template<typename T>
    static Matrix4<T> rotation(const Vector3<T>& rotation);

    template<typename T>
    static Matrix4<T> scale(const Vector3<T>& scale);

    template<typename T>
    static Matrix4<T> camera(const Vector3<T>& eye, const Vector3<T>& view_direction, const Vector3<T>& view_up);

    template<typename T>
    static Matrix4<T> orthogonalProjection(T left, T right, T bottom, T top, T near, T far);

    template<typename T>
    static Matrix4<T> perspectiveProjection(T vertical_fov, T aspect_ratio, T near, T far);

    template<typename T>
    static Matrix4<T> viewport(int32 viewport_width, int32 viewport_height);
};

} // namespace MicroRenderer

#include "Transform.tpp"
