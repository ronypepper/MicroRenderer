//
// Created by pusdo on 18/07/2024.
//

#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

namespace MicroRenderer
{
    template <typename T>
    T radiansToDegrees(T value)
    {
        return value * static_cast<T>(180.0 / M_PI);
    }

    template <typename T>
    T degreeToRadians(T value)
    {
        return value * static_cast<T>(M_PI / 180.0);
    }

    // template<typename T>
    // bool isFlagSet(std::underlying_type_t<T> x, T flag)
    // {
    //     static_assert(std::is_enum_v<T>, "isFlagSet must only be used with enum types!");
    //     using type = std::underlying_type_t<T>;
    //     return static_cast<bool>((x & static_cast<type>(flag)) == static_cast<type>(flag));
    // }
    //
    // template<typename T, std::underlying_type_t<T> x, T flag>
    // constexpr bool check_flag_v = static_cast<bool>((x & static_cast<std::underlying_type_t<T>>(flag)) == static_cast<std::underlying_type_t<T>>(flag));

} // namespace MicroRenderer
