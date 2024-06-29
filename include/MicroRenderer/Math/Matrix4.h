//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "Vector4.h"

namespace MicroRenderer {

// Column-major order
template<typename T>
class Matrix4 {
public:
    // Component values.
    union {
        T components[16];
        Vector4<T> columns[4];
    };

    Matrix4() {}
    explicit Matrix4(T value) {
        components[0] = { value, 0, 0, 0 };
        components[1] = { 0, value, 0, 0 };
        components[2] = { 0, 0, value, 0 };
        components[3] = { 0, 0, 0, value };
    }
    Matrix4(const Vector4<T>& Col_1, const Vector4<T>& Col_2, const Vector4<T>& Col_3, const Vector4<T>& Col_4) {
        columns[0] = Col_1;
        columns[1] = Col_2;
        columns[2] = Col_3;
        columns[3] = Col_4;
    }
    Matrix4(const Matrix4& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
        columns[3] = other.columns[3];
    }

    Matrix4& operator=(const Matrix4& other) {
        columns[0] = other.columns[0];
        columns[1] = other.columns[1];
        columns[2] = other.columns[2];
        columns[3] = other.columns[3];
        return *this;
    }

    // Array subscript operator.
    Vector4<T>& operator[](size_t idx) {
        return columns[idx];
    }
    const Vector4<T>& operator[](size_t idx) const {
        return columns[idx];
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
    Matrix4& operator+=(const Matrix4& rhs) {
        columns[0] += rhs[0];
        columns[1] += rhs[1];
        columns[2] += rhs[2];
        columns[3] += rhs[3];
        return *this;
    }
    friend Matrix4 operator+(Matrix4 lhs, const Matrix4& rhs) {
        lhs += rhs;
        return lhs;
    }
    Matrix4& operator+=(const T& rhs) {
        columns[0] += rhs;
        columns[1] += rhs;
        columns[2] += rhs;
        columns[3] += rhs;
        return *this;
    }
    friend Matrix4 operator+(Matrix4 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend Matrix4 operator+(const T& lhs, Matrix4 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    Matrix4& operator-=(const Matrix4& rhs) {
        columns[0] -= rhs[0];
        columns[1] -= rhs[1];
        columns[2] -= rhs[2];
        columns[3] -= rhs[3];
        return *this;
    }
    friend Matrix4 operator-(Matrix4 lhs, const Matrix4& rhs) {
        lhs -= rhs;
        return lhs;
    }
    Matrix4& operator-=(const T& rhs) {
        columns[0] -= rhs;
        columns[1] -= rhs;
        columns[2] -= rhs;
        columns[3] -= rhs;
        return *this;
    }
    friend Matrix4 operator-(Matrix4 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Matrix-matrix multiplication operators.
    Matrix4& operator*=(const Matrix4& rhs) {
        *this = *this * rhs;
        return *this;
    }
    friend Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs) {
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
    friend Vector4<T> operator*(const Matrix4& lhs, const Vector4<T>& rhs) {
        Vector4<T> result;
        result[0] = rhs[0] * lhs[0][0] + rhs[1] * lhs[1][0] + rhs[2] * lhs[2][0] + rhs[3] * lhs[3][0];
        result[1] = rhs[0] * lhs[0][1] + rhs[1] * lhs[1][1] + rhs[2] * lhs[2][1] + rhs[3] * lhs[3][1];
        result[2] = rhs[0] * lhs[0][2] + rhs[1] * lhs[1][2] + rhs[2] * lhs[2][2] + rhs[3] * lhs[3][2];
        result[3] = rhs[0] * lhs[0][3] + rhs[1] * lhs[1][3] + rhs[2] * lhs[2][3] + rhs[3] * lhs[3][3];
        return result;
    }

    // Matrix-scalar multiplication operators.
    Matrix4& operator*=(const T& rhs) {
        columns[0] *= rhs;
        columns[1] *= rhs;
        columns[2] *= rhs;
        columns[3] *= rhs;
        return *this;
    }
    friend Matrix4 operator*(Matrix4 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend Matrix4 operator*(const T& lhs, Matrix4 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    Matrix4& operator/=(const T& rhs) {
        columns[0] /= rhs;
        columns[1] /= rhs;
        columns[2] /= rhs;
        columns[3] /= rhs;
        return *this;
    }
    friend Matrix4 operator/(Matrix4 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }
};

typedef Matrix4<float> mat4;
typedef Matrix4<double> dmat4;

} // namespace MicroRenderer
