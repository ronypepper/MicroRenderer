//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "ScalarTypes.h"
#include "cmath"

namespace MicroRenderer {

template<typename T>
class Vector3 {
public:
    // Component values.
    union {
        T components[3];

        struct {
            T x;
            T y;
            T z;
        };

        struct {
            T r;
            T g;
            T b;
        };
    };

    Vector3() {}
    explicit Vector3(T value) {
        x = value;
        y = value;
        z = value;
    }
    Vector3(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vector3(const Vector3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    Vector3& operator=(const Vector3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
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
    friend bool operator==(const Vector3 &lhs, const Vector3 &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
    friend bool operator!=(const Vector3 &lhs, const Vector3 &rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(const Vector3 &lhs, const Vector3 &rhs) {
        return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
    }
    friend bool operator<=(const Vector3 &lhs, const Vector3 &rhs) {
        return !(lhs > rhs);
    }
    friend bool operator>(const Vector3 &lhs, const Vector3 &rhs) {
        return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z;
    }
    friend bool operator>=(const Vector3 &lhs, const Vector3 &rhs) {
        return !(lhs < rhs);
    }

    // Addition operators.
    Vector3& operator+=(const Vector3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    friend Vector3 operator+(Vector3 lhs, const Vector3& rhs) {
        lhs += rhs;
        return lhs;
    }
    Vector3& operator+=(const T& rhs) {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }
    friend Vector3 operator+(Vector3 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend Vector3 operator+(const T& lhs, Vector3 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    Vector3& operator-=(const Vector3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    friend Vector3 operator-(Vector3 lhs, const Vector3& rhs) {
        lhs -= rhs;
        return lhs;
    }
    Vector3& operator-=(const T& rhs) {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }
    friend Vector3 operator-(Vector3 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Multiplication operators.
    Vector3& operator*=(const Vector3& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }
    friend Vector3 operator*(Vector3 lhs, const Vector3& rhs) {
        lhs *= rhs;
        return lhs;
    }
    Vector3& operator*=(const T& rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    friend Vector3 operator*(Vector3 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend Vector3 operator*(const T& lhs, Vector3 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    Vector3& operator/=(const Vector3& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    friend Vector3 operator/(Vector3 lhs, const Vector3& rhs) {
        lhs /= rhs;
        return lhs;
    }
    Vector3& operator/=(const T& rhs) {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }
    friend Vector3 operator/(Vector3 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // Scalar product.
    T dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Scalar product.
    Vector3 cross(const Vector3& other) const {
        return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
    }

    // Get squared length.
    T squaredLength() const {
        return x * x + y * y + z * z;
    }

    // Get length.
    T length() const {
        return std::sqrt(x * x + y * y + z * z);
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
    Vector3 getNormalized(T epsilon = 0.001) const {
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
        return max;
    }
    T min() const {
        T min = x < y ? x : y;
        min = z < min ? z : min;
        return min;
    }
};

typedef Vector3<float> vec3;
typedef Vector3<double> dvec3;
typedef Vector3<int32> ivec3;
typedef Vector3<uint32> uvec3;
typedef Vector3<bool> bvec3;

} // namespace MicroRenderer
