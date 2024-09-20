//
// Created by pusdo on 28/06/2024.
//

#pragma once
#include "ScalarTypes.h"
#include "cmath"

namespace MicroRenderer {

template<typename T>
class Vector2 {
public:
    static_assert(std::is_arithmetic_v<T>, "Template type of Vector2<T> must be arithmetic!");

    // Component values.
    union {
        T components[2];

        struct {
            T x;
            T y;
        };

        struct {
            T r;
            T g;
        };
        
        struct {
            T u;
            T v;
        };
    };

    // Constructors.
    Vector2() {}
    constexpr Vector2(T value) {
        x = value;
        y = value;
    }
    constexpr Vector2(T x, T y) {
        this->x = x;
        this->y = y;
    }
    constexpr Vector2(const Vector2& other) {
        x = other.x;
        y = other.y;
    }

    // Conversion operator.
    template<typename U>
    explicit constexpr operator Vector2<U>() const
    {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    // Assignment operator.
    Vector2& operator=(const Vector2& other) {
        x = other.x;
        y = other.y;
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
    friend bool operator==(const Vector2 &lhs, const Vector2 &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    friend bool operator!=(const Vector2 &lhs, const Vector2 &rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(const Vector2 &lhs, const Vector2 &rhs) {
        return lhs.x < rhs.x && lhs.y < rhs.y;
    }
    friend bool operator<=(const Vector2 &lhs, const Vector2 &rhs) {
        return !(lhs > rhs);
    }
    friend bool operator>(const Vector2 &lhs, const Vector2 &rhs) {
        return lhs.x > rhs.x && lhs.y > rhs.y;
    }
    friend bool operator>=(const Vector2 &lhs, const Vector2 &rhs) {
        return !(lhs < rhs);
    }

    // Addition operators.
    constexpr Vector2& operator+=(const Vector2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    friend constexpr Vector2 operator+(Vector2 lhs, const Vector2& rhs) {
        lhs += rhs;
        return lhs;
    }
    constexpr Vector2& operator+=(const T& rhs) {
        x += rhs;
        y += rhs;
        return *this;
    }
    friend constexpr Vector2 operator+(Vector2 lhs, const T& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend constexpr Vector2 operator+(const T& lhs, Vector2 rhs) {
        rhs += lhs;
        return rhs;
    }

    // Subtraction operators.
    constexpr Vector2& operator-=(const Vector2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    friend constexpr Vector2 operator-(Vector2 lhs, const Vector2& rhs) {
        lhs -= rhs;
        return lhs;
    }
    constexpr Vector2& operator-=(const T& rhs) {
        x -= rhs;
        y -= rhs;
        return *this;
    }
    friend constexpr Vector2 operator-(Vector2 lhs, const T& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Negative operator.
    constexpr Vector2 operator-() const {
        return {-x, -y};
    }

    // Multiplication operators.
    constexpr Vector2& operator*=(const Vector2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }
    friend constexpr Vector2 operator*(Vector2 lhs, const Vector2& rhs) {
        lhs *= rhs;
        return lhs;
    }
    constexpr Vector2& operator*=(const T& rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    friend constexpr Vector2 operator*(Vector2 lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend constexpr Vector2 operator*(const T& lhs, Vector2 rhs) {
        rhs *= lhs;
        return rhs;
    }

    // Division operators.
    constexpr Vector2& operator/=(const Vector2& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }
    friend constexpr Vector2 operator/(Vector2 lhs, const Vector2& rhs) {
        lhs /= rhs;
        return lhs;
    }
    constexpr Vector2& operator/=(const T& rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }
    friend constexpr Vector2 operator/(Vector2 lhs, const T& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // Scalar product.
    T dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Get squared length.
    T squaredLength() const {
        return x * x + y * y;
    }

    // Get length.
    T length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalization with zero-check.
    void normalizeSafe(T epsilon = static_cast<T>(0.001)) {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            *this /= std::sqrt(sqLength);
        }
    }

    // Normalization without zero-check.
    void normalizeUnsafe() {
        *this /= length();
    }

    // Return normalized copy with zero-check, returns zero-vector on fail.
    Vector2 getNormalized(T epsilon = static_cast<T>(0.001)) const {
        T sqLength = squaredLength();
        if (sqLength > epsilon) {
            return *this / std::sqrt(sqLength);
        }
        return {static_cast<T>(0.0)};
    }

    // Max and Min of components.
    T max() const {
        return x > y ? x : y;
    }
    T min() const {
        return x < y ? x : y;
    }

    // Returns per-component max and min of a vector and a scalar.
    static Vector2 max(const Vector2& v, T s) {
        return {std::max(v.x, s), std::max(v.y, s)};
    }
    static Vector2 min(const Vector2& v, T s) {
        return {std::min(v.x, s), std::min(v.y, s)};
    }

    // Returns per-component max and min of two vectors.
    static Vector2 max(const Vector2& v1, const Vector2& v2) {
        return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
    }
    static Vector2 min(const Vector2& v1, const Vector2& v2) {
        return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
    }
};

typedef Vector2<float> vec2;
typedef Vector2<double> dvec2;
typedef Vector2<int32> ivec2;
typedef Vector2<uint32> uvec2;
typedef Vector2<bool> bvec2;

} // namespace MicroRenderer
