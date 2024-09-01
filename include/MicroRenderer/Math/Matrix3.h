//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Vector3.h"

namespace MicroRenderer {

// Column-major order
template<typename T>
class Matrix3 {
public:
    static_assert(std::is_arithmetic_v<T>, "Template type of Matrix3<T> must be arithmetic!");

    // Component values.
    union {
        T components[9];
        Vector3<T> columns[3];
    };

    // Constructors.
    Matrix3() {}
    constexpr Matrix3(T x0, T y0, T z0, T x1, T y1, T z1, T x2, T y2, T z2) {
        components[0] = x0;
        components[1] = y0;
        components[2] = z0;
        components[3] = x1;
        components[4] = y1;
        components[5] = z1;
        components[6] = x2;
        components[7] = y2;
        components[8] = z2;
    }
    constexpr Matrix3(T value) {
        columns[0] = {value, static_cast<T>(0.0), static_cast<T>(0.0)};
        columns[1] = {static_cast<T>(0.0), value, static_cast<T>(0.0)};
        columns[2] = {static_cast<T>(0.0), static_cast<T>(0.0), value};
    }
    constexpr Matrix3(const Vector3<T>& col_1, const Vector3<T>& col_2, const Vector3<T>& col_3) {
        columns[0] = col_1;
        columns[1] = col_2;
        columns[2] = col_3;
    }
    constexpr Matrix3(const Matrix3& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
    }

    // Conversion operator.
    template<typename U>
    explicit constexpr operator Matrix3<U>() const
    {
        return {
            static_cast<Vector3<U>>(columns[0]), static_cast<Vector3<U>>(columns[1]),
            static_cast<Vector3<U>>(columns[2])
        };
    }

    // Assignment operator.
    Matrix3& operator=(const Matrix3& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
        return *this;
    }

    // Array subscript operator.
    Vector3<T>& operator[](size_t idx) {
        assert(idx >= 0 && idx <= 2);
        return columns[idx];
    }
    const Vector3<T>& operator[](size_t idx) const {
        assert(idx >= 0 && idx <= 2);
        return columns[idx];
    }

    // Relational operators.
    friend bool operator==(const Matrix3 &lhs, const Matrix3 &rhs) {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
    }
    friend bool operator!=(const Matrix3 &lhs, const Matrix3 &rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(const Matrix3 &lhs, const Matrix3 &rhs) {
        return lhs[0] < rhs[0] && lhs[1] < rhs[1] && lhs[2] < rhs[2];
    }
    friend bool operator<=(const Matrix3 &lhs, const Matrix3 &rhs) {
        return !(lhs > rhs);
    }
    friend bool operator>(const Matrix3 &lhs, const Matrix3 &rhs) {
        return lhs[0] > rhs[0] && lhs[1] > rhs[1] && lhs[2] > rhs[2];
    }
    friend bool operator>=(const Matrix3 &lhs, const Matrix3 &rhs) {
        return !(lhs < rhs);
    }

    // Addition operators.
    constexpr Matrix3& operator+=(const Matrix3& rhs) {
        columns[0] += rhs[0];
        columns[1] += rhs[1];
        columns[2] += rhs[2];
        return *this;
    }
    friend constexpr Matrix3 operator+(Matrix3 lhs, const Matrix3& rhs) {
        lhs += rhs;
        return lhs;
    }
    constexpr Matrix3& operator+=(const T& rhs) {
        columns[0] += rhs;
        columns[1] += rhs;
        columns[2] += rhs;
        return *this;
    }
    friend constexpr Matrix3 operator+(Matrix3 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend constexpr Matrix3 operator+(const T& lhs, Matrix3 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    constexpr Matrix3& operator-=(const Matrix3& rhs) {
        columns[0] -= rhs[0];
        columns[1] -= rhs[1];
        columns[2] -= rhs[2];
        return *this;
    }
    friend constexpr Matrix3 operator-(Matrix3 lhs, const Matrix3& rhs) {
        lhs -= rhs;
        return lhs;
    }
    constexpr Matrix3& operator-=(const T& rhs) {
        columns[0] -= rhs;
        columns[1] -= rhs;
        columns[2] -= rhs;
        return *this;
    }
    friend constexpr Matrix3 operator-(Matrix3 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Matrix-matrix multiplication operators.
    constexpr Matrix3& operator*=(const Matrix3& rhs) {
        *this = *this * rhs;
        return *this;
    }
    friend constexpr Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs) {
        Matrix3 result;
        result[0][0] = rhs[0][0] * lhs[0][0] + rhs[0][1] * lhs[1][0] + rhs[0][2] * lhs[2][0];
        result[0][1] = rhs[0][0] * lhs[0][1] + rhs[0][1] * lhs[1][1] + rhs[0][2] * lhs[2][1];
        result[0][2] = rhs[0][0] * lhs[0][2] + rhs[0][1] * lhs[1][2] + rhs[0][2] * lhs[2][2];
        result[1][0] = rhs[1][0] * lhs[0][0] + rhs[1][1] * lhs[1][0] + rhs[1][2] * lhs[2][0];
        result[1][1] = rhs[1][0] * lhs[0][1] + rhs[1][1] * lhs[1][1] + rhs[1][2] * lhs[2][1];
        result[1][2] = rhs[1][0] * lhs[0][2] + rhs[1][1] * lhs[1][2] + rhs[1][2] * lhs[2][2];
        result[2][0] = rhs[2][0] * lhs[0][0] + rhs[2][1] * lhs[1][0] + rhs[2][2] * lhs[2][0];
        result[2][1] = rhs[2][0] * lhs[0][1] + rhs[2][1] * lhs[1][1] + rhs[2][2] * lhs[2][1];
        result[2][2] = rhs[2][0] * lhs[0][2] + rhs[2][1] * lhs[1][2] + rhs[2][2] * lhs[2][2];
        return result;
    }

    // Matrix-vector multiplication operator.
    friend constexpr Vector3<T> operator*(const Matrix3& lhs, const Vector3<T>& rhs) {
        Vector3<T> result;
        result[0] = rhs[0] * lhs[0][0] + rhs[1] * lhs[1][0] + rhs[2] * lhs[2][0];
        result[1] = rhs[0] * lhs[0][1] + rhs[1] * lhs[1][1] + rhs[2] * lhs[2][1];
        result[2] = rhs[0] * lhs[0][2] + rhs[1] * lhs[1][2] + rhs[2] * lhs[2][2];
        return result;
    }

    // Matrix-scalar multiplication operators.
    constexpr Matrix3& operator*=(const T& rhs) {
        columns[0] *= rhs;
        columns[1] *= rhs;
        columns[2] *= rhs;
        return *this;
    }
    friend constexpr Matrix3 operator*(Matrix3 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend constexpr Matrix3 operator*(const T& lhs, Matrix3 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    constexpr Matrix3& operator/=(const T& rhs) {
        columns[0] /= rhs;
        columns[1] /= rhs;
        columns[2] /= rhs;
        return *this;
    }
    friend constexpr Matrix3 operator/(Matrix3 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // Return transpose of matrix.
    Matrix3 getTranspose() const
    {
        return {
            components[0], components[3], components[6],
            components[1], components[4], components[7],
            components[2], components[5], components[8]
        };
    }

};

typedef Matrix3<float> mat3;
typedef Matrix3<double> dmat3;

} // namespace MicroRenderer
