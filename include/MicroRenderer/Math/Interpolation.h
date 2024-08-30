//
// Created by pusdo on 18/08/2024.
//

#pragma once
#include "MicroRenderer/Math/Vector2.h"

namespace MicroRenderer {

// Linear interpolation.

template<typename T, typename Attribute>
    Attribute interpolateLinearly(const Attribute& from, const Attribute& to, T from_factor, T to_factor)
{
    return from * from_factor + to * to_factor;
}

// Barycentric interpolation.

template<typename T>
struct BarycentricIncrements
{
    Vector2<T> alpha;
    Vector2<T> beta;
    Vector2<T> gamma;
};

template<typename T>
void computeBarycentricIncrements(const Vector2<T>& pos_1, const Vector2<T>& pos_2, const Vector2<T>& pos_3,
                                  BarycentricIncrements<T>& bc_incs)
{
    const T alpha_signed_dist = (pos_2.y - pos_3.y) * pos_1.x + (pos_3.x - pos_2.x) * pos_1.y + pos_2.x * pos_3.y - pos_3.x * pos_2.y;
    bc_incs.alpha = {(pos_2.y - pos_3.y) / alpha_signed_dist, (pos_3.x - pos_2.x) / alpha_signed_dist};
    const T beta_signed_dist = (pos_3.y - pos_1.y) * pos_2.x + (pos_1.x - pos_3.x) * pos_2.y + pos_3.x * pos_1.y - pos_1.x * pos_3.y;
    bc_incs.beta = {(pos_3.y - pos_1.y) / beta_signed_dist, (pos_1.x - pos_3.x) / beta_signed_dist};
    const T gamma_signed_dist = (pos_1.y - pos_2.y) * pos_3.x + (pos_2.x - pos_1.x) * pos_3.y + pos_1.x * pos_2.y - pos_2.x * pos_1.y;
    bc_incs.gamma = {(pos_1.y - pos_2.y) / gamma_signed_dist, (pos_2.x - pos_1.x) / gamma_signed_dist};
}

enum class IncrementationMode : uint32
{
    OneInX,
    OneInY,
    OffsetInX,
    OffsetInY
};

template<typename T, typename AttrType>
class TriangleAttribute
{
public:
    void initialize(const AttrType& v1, const AttrType& v2, const AttrType& v3, const BarycentricIncrements<T>& bc_incs,
                    const Vector2<T>& offset)
    {
        increment_x = v1 * bc_incs.alpha.x + v2 * bc_incs.beta.x + v3 * bc_incs.gamma.x;
        increment_y = v1 * bc_incs.alpha.y + v2 * bc_incs.beta.y + v3 * bc_incs.gamma.y;
        current_value = v1 + increment_x * offset.x + increment_y * offset.y;
    }

    AttrType getValue()
    {
        return current_value;
    }

    template<IncrementationMode mode>
    void increment(int32 offset = 1)
    {
        if constexpr(mode == IncrementationMode::OneInX) {
            current_value += increment_x;
        }
        else if constexpr(mode == IncrementationMode::OneInY) {
            current_value += increment_y;
        }
        else if constexpr(mode == IncrementationMode::OffsetInX) {
            current_value += increment_x * static_cast<T>(offset);
        }
        else if constexpr(mode == IncrementationMode::OffsetInY) {
            current_value += increment_y * static_cast<T>(offset);
        }
    }
private:
    AttrType current_value;
    AttrType increment_x;
    AttrType increment_y;
};

// template<typename SourceType, typename WorkType> POSSIBLE_FEATURE: selectable float/fp and precision through RendererConfiguration
// class TriangleAttribute
// {
// public:
//     enum class IncrementationMode : uint32
//     {
//         OneInX,
//         OneInY,
//         OffsetInX,
//         OffsetInY
//     };
//
//     TriangleAttribute(const SourceType& v1, const SourceType& v2, const SourceType& v3,
//                       const BarycentricIncrements<WorkType>& bc_incs, const Vector2<WorkType>& offset)
//     {
//         increment_x = static_cast<WorkType>(v1) * bc_incs.alpha.x + static_cast<WorkType>(v2) * bc_incs.beta.x +
//                       static_cast<WorkType>(v3) * bc_incs.gamma.x;
//         increment_y = static_cast<WorkType>(v1) * bc_incs.alpha.y + static_cast<WorkType>(v2) * bc_incs.beta.y +
//                       static_cast<WorkType>(v3) * bc_incs.gamma.y;
//         current_value = static_cast<WorkType>(v1) + increment_x * offset.x + increment_y * offset.y;
//     }
//
//     SourceType getValue()
//     {
//         return static_cast<SourceType>(current_value);
//     }
//
//     template<IncrementationMode mode>
//     void increment(int32 offset = 1)
//     {
//         if constexpr(mode == IncrementationMode::OneInX) {
//             current_value += increment_x;
//         }
//         else if constexpr(mode == IncrementationMode::OneInY) {
//             current_value += increment_y;
//         }
//         else if constexpr(mode == IncrementationMode::OffsetInX) {
//             current_value += increment_x * static_cast<WorkType>(offset);
//         }
//         else if constexpr(mode == IncrementationMode::OffsetInY) {
//             current_value += increment_y * static_cast<WorkType>(offset);
//         }
//     }
// private:
//     WorkType current_value;
//     WorkType increment_x;
//     WorkType increment_y;
// };

} // namespace MicroRenderer
