//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "ScalarTypes.h"
#include "Vector2.h"
#include "cmath"

namespace MicroRenderer {

template<typename T>
class Vector3 {
public:
    static_assert(std::is_arithmetic_v<T>, "Template type of Vector3<T> must be arithmetic!");

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

    // Constructors.
    Vector3() {}
    constexpr Vector3(T value) {
        x = value;
        y = value;
        z = value;
    }
    constexpr Vector3(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    constexpr Vector3(const Vector3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    constexpr Vector3(const Vector2<T>& xy, T z) {
        this->x = xy.x;
        this->y = xy.y;
        this->z = z;
    }
    constexpr Vector3(T x, const Vector2<T>& yz) {
        this->x = x;
        this->y = yz.x;
        this->z = yz.y;
    }

    // Conversion operator.
    template<typename U>
    explicit constexpr operator Vector3<U>() const
    {
        return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
    }

    // Assignment operator.
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

    // Sub-vector access.
    Vector2<T> getXY() const
    {
        return Vector2<T>(x, y);
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
    constexpr Vector3& operator+=(const Vector3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    friend constexpr Vector3 operator+(Vector3 lhs, const Vector3& rhs) {
        lhs += rhs;
        return lhs;
    }
    constexpr Vector3& operator+=(const T& rhs) {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }
    friend constexpr Vector3 operator+(Vector3 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend constexpr Vector3 operator+(const T& lhs, Vector3 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    constexpr Vector3& operator-=(const Vector3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    friend constexpr Vector3 operator-(Vector3 lhs, const Vector3& rhs) {
        lhs -= rhs;
        return lhs;
    }
    constexpr Vector3& operator-=(const T& rhs) {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }
    friend constexpr Vector3 operator-(Vector3 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Negative operator.
    constexpr Vector3 operator-() const {
        return {-x, -y, -z};
    }

    // Multiplication operators.
    constexpr Vector3& operator*=(const Vector3& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }
    friend constexpr Vector3 operator*(Vector3 lhs, const Vector3& rhs) {
        lhs *= rhs;
        return lhs;
    }
    constexpr Vector3& operator*=(const T& rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    friend constexpr Vector3 operator*(Vector3 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend constexpr Vector3 operator*(const T& lhs, Vector3 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    constexpr Vector3& operator/=(const Vector3& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    friend constexpr Vector3 operator/(Vector3 lhs, const Vector3& rhs) {
        lhs /= rhs;
        return lhs;
    }
    constexpr Vector3& operator/=(const T& rhs) {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }
    friend constexpr Vector3 operator/(Vector3 lhs, const T& rhs) {
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
    void normalizeSafe(T epsilon = static_cast<T>(0.001)) {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            *this /= sqLength;
        }
        else {
            *this = {static_cast<T>(0.0)};
        }
    }

    // Normalization without zero-check.
    void normalizeUnsafe() {
        *this /= squaredLength();
    }

    // Return normalized copy with zero-check, returns zero-vector on fail.
    Vector3 getNormalized(T epsilon = static_cast<T>(0.001)) const {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            return *this / sqLength;
        }
        return {static_cast<T>(0.0)};
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
