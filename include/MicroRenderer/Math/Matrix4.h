//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Matrix3.h"
#include "Vector4.h"

namespace MicroRenderer {

// Column-major order
template<typename T>
class Matrix4 {
public:
    static_assert(std::is_arithmetic_v<T>, "Template type of Matrix4<T> must be arithmetic!");

    // Component values.
    union {
        T components[16];
        Vector4<T> columns[4];
    };

    // Constructors.
    Matrix4() {}
    constexpr Matrix4(T x0, T y0, T z0, T w0, T x1, T y1, T z1, T w1, T x2, T y2, T z2, T w2, T x3, T y3, T z3, T w3) {
        components[0] = x0;
        components[1] = y0;
        components[2] = z0;
        components[3] = w0;
        components[4] = x1;
        components[5] = y1;
        components[6] = z1;
        components[7] = w1;
        components[8] = x2;
        components[9] = y2;
        components[10] = z2;
        components[11] = w2;
        components[12] = x3;
        components[13] = y3;
        components[14] = z3;
        components[15] = w3;
    }
    constexpr Matrix4(T value) {
        columns[0] = {value, static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0)};
        columns[1] = {static_cast<T>(0.0), value, static_cast<T>(0.0), static_cast<T>(0.0)};
        columns[2] = {static_cast<T>(0.0), static_cast<T>(0.0), value, static_cast<T>(0.0)};
        columns[3] = {static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), value};
    }
    constexpr Matrix4(const Vector4<T>& col_1, const Vector4<T>& col_2, const Vector4<T>& col_3, const Vector4<T>& col_4) {
        columns[0] = col_1;
        columns[1] = col_2;
        columns[2] = col_3;
        columns[3] = col_4;
    }
    constexpr Matrix4(const Matrix4& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
        columns[3] = other.columns[3];
    }

    // Conversion operator.
    template<typename U>
    explicit constexpr operator Matrix4<U>() const
    {
        return {
            static_cast<Vector4<U>>(columns[0]), static_cast<Vector4<U>>(columns[1]),
            static_cast<Vector4<U>>(columns[2]), static_cast<Vector4<U>>(columns[3])
        };
    }

    // Assignment operator.
    Matrix4& operator=(const Matrix4& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
        columns[3] = other.columns[3];
        return *this;
    }

    // Array subscript operator.
    Vector4<T>& operator[](size_t idx) {
        assert(idx >= 0 && idx <= 3);
        return columns[idx];
    }
    const Vector4<T>& operator[](size_t idx) const {
        assert(idx >= 0 && idx <= 3);
        return columns[idx];
    }

    // Sub-matrix access.
    Matrix3<T> getMatrix3() const
    {
        return {
            components[0], components[1], components[2],
            components[4], components[5], components[6],
            components[8], components[9], components[10]
        };
    }

    // Relational operators.
    friend bool operator==(const Matrix4 &lhs, const Matrix4 &rhs) {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
    }
    friend bool operator!=(const Matrix4 &lhs, const Matrix4 &rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(const Matrix4 &lhs, const Matrix4 &rhs) {
        return lhs[0] < rhs[0] && lhs[1] < rhs[1] && lhs[2] < rhs[2] && lhs[3] < rhs[3];
    }
    friend bool operator<=(const Matrix4 &lhs, const Matrix4 &rhs) {
        return !(lhs > rhs);
    }
    friend bool operator>(const Matrix4 &lhs, const Matrix4 &rhs) {
        return lhs[0] > rhs[0] && lhs[1] > rhs[1] && lhs[2] > rhs[2] && lhs[3] > rhs[3];
    }
    friend bool operator>=(const Matrix4 &lhs, const Matrix4 &rhs) {
        return !(lhs < rhs);
    }

    // Addition operators.
    constexpr Matrix4& operator+=(const Matrix4& rhs) {
        columns[0] += rhs[0];
        columns[1] += rhs[1];
        columns[2] += rhs[2];
        columns[3] += rhs[3];
        return *this;
    }
    friend constexpr Matrix4 operator+(Matrix4 lhs, const Matrix4& rhs) {
        lhs += rhs;
        return lhs;
    }
    constexpr Matrix4& operator+=(const T& rhs) {
        columns[0] += rhs;
        columns[1] += rhs;
        columns[2] += rhs;
        columns[3] += rhs;
        return *this;
    }
    friend constexpr Matrix4 operator+(Matrix4 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend constexpr Matrix4 operator+(const T& lhs, Matrix4 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    constexpr Matrix4& operator-=(const Matrix4& rhs) {
        columns[0] -= rhs[0];
        columns[1] -= rhs[1];
        columns[2] -= rhs[2];
        columns[3] -= rhs[3];
        return *this;
    }
    friend constexpr Matrix4 operator-(Matrix4 lhs, const Matrix4& rhs) {
        lhs -= rhs;
        return lhs;
    }
    constexpr Matrix4& operator-=(const T& rhs) {
        columns[0] -= rhs;
        columns[1] -= rhs;
        columns[2] -= rhs;
        columns[3] -= rhs;
        return *this;
    }
    friend constexpr Matrix4 operator-(Matrix4 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Matrix-matrix multiplication operators.
    constexpr Matrix4& operator*=(const Matrix4& rhs) {
        *this = *this * rhs;
        return *this;
    }
    friend constexpr Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs) {
        Matrix4 result;
        result[0][0] = rhs[0][0] * lhs[0][0] + rhs[0][1] * lhs[1][0] + rhs[0][2] * lhs[2][0] + rhs[0][3] * lhs[3][0];
        result[0][1] = rhs[0][0] * lhs[0][1] + rhs[0][1] * lhs[1][1] + rhs[0][2] * lhs[2][1] + rhs[0][3] * lhs[3][1];
        result[0][2] = rhs[0][0] * lhs[0][2] + rhs[0][1] * lhs[1][2] + rhs[0][2] * lhs[2][2] + rhs[0][3] * lhs[3][2];
        result[0][3] = rhs[0][0] * lhs[0][3] + rhs[0][1] * lhs[1][3] + rhs[0][2] * lhs[2][3] + rhs[0][3] * lhs[3][3];
        result[1][0] = rhs[1][0] * lhs[0][0] + rhs[1][1] * lhs[1][0] + rhs[1][2] * lhs[2][0] + rhs[1][3] * lhs[3][0];
        result[1][1] = rhs[1][0] * lhs[0][1] + rhs[1][1] * lhs[1][1] + rhs[1][2] * lhs[2][1] + rhs[1][3] * lhs[3][1];
        result[1][2] = rhs[1][0] * lhs[0][2] + rhs[1][1] * lhs[1][2] + rhs[1][2] * lhs[2][2] + rhs[1][3] * lhs[3][2];
        result[1][3] = rhs[1][0] * lhs[0][3] + rhs[1][1] * lhs[1][3] + rhs[1][2] * lhs[2][3] + rhs[1][3] * lhs[3][3];
        result[2][0] = rhs[2][0] * lhs[0][0] + rhs[2][1] * lhs[1][0] + rhs[2][2] * lhs[2][0] + rhs[2][3] * lhs[3][0];
        result[2][1] = rhs[2][0] * lhs[0][1] + rhs[2][1] * lhs[1][1] + rhs[2][2] * lhs[2][1] + rhs[2][3] * lhs[3][1];
        result[2][2] = rhs[2][0] * lhs[0][2] + rhs[2][1] * lhs[1][2] + rhs[2][2] * lhs[2][2] + rhs[2][3] * lhs[3][2];
        result[2][3] = rhs[2][0] * lhs[0][3] + rhs[2][1] * lhs[1][3] + rhs[2][2] * lhs[2][3] + rhs[2][3] * lhs[3][3];
        result[3][0] = rhs[3][0] * lhs[0][0] + rhs[3][1] * lhs[1][0] + rhs[3][2] * lhs[2][0] + rhs[3][3] * lhs[3][0];
        result[3][1] = rhs[3][0] * lhs[0][1] + rhs[3][1] * lhs[1][1] + rhs[3][2] * lhs[2][1] + rhs[3][3] * lhs[3][1];
        result[3][2] = rhs[3][0] * lhs[0][2] + rhs[3][1] * lhs[1][2] + rhs[3][2] * lhs[2][2] + rhs[3][3] * lhs[3][2];
        result[3][3] = rhs[3][0] * lhs[0][3] + rhs[3][1] * lhs[1][3] + rhs[3][2] * lhs[2][3] + rhs[3][3] * lhs[3][3];
        return result;
    }

    // Matrix-vector multiplication operator.
    friend constexpr Vector4<T> operator*(const Matrix4& lhs, const Vector4<T>& rhs) {
        Vector4<T> result;
        result[0] = rhs[0] * lhs[0][0] + rhs[1] * lhs[1][0] + rhs[2] * lhs[2][0] + rhs[3] * lhs[3][0];
        result[1] = rhs[0] * lhs[0][1] + rhs[1] * lhs[1][1] + rhs[2] * lhs[2][1] + rhs[3] * lhs[3][1];
        result[2] = rhs[0] * lhs[0][2] + rhs[1] * lhs[1][2] + rhs[2] * lhs[2][2] + rhs[3] * lhs[3][2];
        result[3] = rhs[0] * lhs[0][3] + rhs[1] * lhs[1][3] + rhs[2] * lhs[2][3] + rhs[3] * lhs[3][3];
        return result;
    }

    // Matrix-scalar multiplication operators.
    constexpr Matrix4& operator*=(const T& rhs) {
        columns[0] *= rhs;
        columns[1] *= rhs;
        columns[2] *= rhs;
        columns[3] *= rhs;
        return *this;
    }
    friend constexpr Matrix4 operator*(Matrix4 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend constexpr Matrix4 operator*(const T& lhs, Matrix4 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    constexpr Matrix4& operator/=(const T& rhs) {
        columns[0] /= rhs;
        columns[1] /= rhs;
        columns[2] /= rhs;
        columns[3] /= rhs;
        return *this;
    }
    friend constexpr Matrix4 operator/(Matrix4 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // Return transpose of matrix.
    Matrix4 getTranspose() const
    {
        return {
            components[0], components[4], components[8], components[12],
            components[1], components[5], components[9], components[13],
            components[2], components[6], components[10], components[14],
            components[3], components[7], components[11], components[15]
        };
    }

    // Performs a matrix-vector multiplication with a homogenous 3D-vector, i.e. matrix * {x, y, z, 1}, skipping the multiplications by one.
    Vector4<T> transformPosition(const Vector3<T>& vector) const
    {
        Vector4<T> result;
        result[0] = vector[0] * columns[0][0] + vector[1] * columns[1][0] + vector[2] * columns[2][0] + columns[3][0];
        result[1] = vector[0] * columns[0][1] + vector[1] * columns[1][1] + vector[2] * columns[2][1] + columns[3][1];
        result[2] = vector[0] * columns[0][2] + vector[1] * columns[1][2] + vector[2] * columns[2][2] + columns[3][2];
        result[3] = vector[0] * columns[0][3] + vector[1] * columns[1][3] + vector[2] * columns[2][3] + columns[3][3];
        return result;
    }
};

typedef Matrix4<float> mat4;
typedef Matrix4<double> dmat4;

} // namespace MicroRenderer
