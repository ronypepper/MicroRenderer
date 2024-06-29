//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "ScalarTypes.h"
#include "cmath"

namespace MicroRenderer {

template<typename T>
class Vector4 {
public:
    // Component values.
    union {
        T components[4];

        struct {
            T x;
            T y;
            T z;
            T w;
        };

        struct {
            T r;
            T g;
            T b;
            T a;
        };
    };

    Vector4() {}
    explicit Vector4(T value) {
        x = value;
        y = value;
        z = value;
        w = value;
    }
    Vector4(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    Vector4(const Vector4& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
    }

    Vector4& operator=(const Vector4& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    // Array subscript operator.
    T& operator[](size_t idx) {
        return components[idx];
    }
    const T& operator[](size_t idx) const {
        return components[idx];
    }

    // Relational operators.
    friend bool operator==(const Vector4 &lhs, const Vector4 &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }
    friend bool operator!=(const Vector4 &lhs, const Vector4 &rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(const Vector4 &lhs, const Vector4 &rhs) {
        return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z && lhs.w < rhs.w;
    }
    friend bool operator<=(const Vector4 &lhs, const Vector4 &rhs) {
        return !(lhs > rhs);
    }
    friend bool operator>(const Vector4 &lhs, const Vector4 &rhs) {
        return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z && lhs.w > rhs.w;
    }
    friend bool operator>=(const Vector4 &lhs, const Vector4 &rhs) {
        return !(lhs < rhs);
    }

    // Addition operators.
    Vector4& operator+=(const Vector4& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }
    friend Vector4 operator+(Vector4 lhs, const Vector4& rhs) {
        lhs += rhs;
        return lhs;
    }
    Vector4& operator+=(const T& rhs) {
        x += rhs;
        y += rhs;
        z += rhs;
        w += rhs;
        return *this;
    }
    friend Vector4 operator+(Vector4 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend Vector4 operator+(const T& lhs, Vector4 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    Vector4& operator-=(const Vector4& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }
    friend Vector4 operator-(Vector4 lhs, const Vector4& rhs) {
        lhs -= rhs;
        return lhs;
    }
    Vector4& operator-=(const T& rhs) {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        w -= rhs;
        return *this;
    }
    friend Vector4 operator-(Vector4 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Multiplication operators.
    Vector4& operator*=(const Vector4& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }
    friend Vector4 operator*(Vector4 lhs, const Vector4& rhs) {
        lhs *= rhs;
        return lhs;
    }
    Vector4& operator*=(const T& rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        w *= rhs;
        return *this;
    }
    friend Vector4 operator*(Vector4 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend Vector4 operator*(const T& lhs, Vector4 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    Vector4& operator/=(const Vector4& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }
    friend Vector4 operator/(Vector4 lhs, const Vector4& rhs) {
        lhs /= rhs;
        return lhs;
    }
    Vector4& operator/=(const T& rhs) {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        w /= rhs;
        return *this;
    }
    friend Vector4 operator/(Vector4 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // Scalar product.
    T dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // Get squared length.
    T squaredLength() const {
        return x * x + y * y + z * z + w * w;
    }

    // Get length.
    T length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    // Normalization with zero-check.
    void normalizeSafe(T epsilon = 0.001) {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            *this /= sqLength;
        }
    }

    // Normalization without zero-check.
    void normalizeUnsafe() {
        *this /= squaredLength();
    }

    // Return normalized copy with zero-check, returns zero-vector on fail.
    Vector4 getNormalized(T epsilon = 0.001) const {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            return *this / sqLength;
        }
        return {0};
    }

    // Max and Min of components.
    T max() const {
        T max = x > y ? x : y;
        max = z > max ? z : max;
        max = w > max ? w : max;
        return max;
    }
    T min() const {
        T min = x < y ? x : y;
        min = z < min ? z : min;
        min = w < min ? w : min;
        return min;
    }
};

typedef Vector4<float> vec4;
typedef Vector4<double> dvec4;
typedef Vector4<int32> ivec4;
typedef Vector4<uint32> uvec4;
typedef Vector4<bool> bvec4;

} // namespace MicroRenderer
