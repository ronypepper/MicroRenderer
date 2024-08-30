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

template<typename Attribute>
struct AttributeIncrements
{
    Attribute x;
    Attribute y;
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

template<typename T, typename Attribute>
void computeAttributeIncrements(const Attribute& attr_1, const Attribute& attr_2, const Attribute& attr_3,
                                const BarycentricIncrements<T>& bc_incs, AttributeIncrements<Attribute>& attr_incs)
{
    attr_incs.x = attr_1 * bc_incs.alpha.x + attr_2 * bc_incs.beta.x + attr_3 * bc_incs.gamma.x;
    attr_incs.y = attr_1 * bc_incs.alpha.y + attr_2 * bc_incs.beta.y + attr_3 * bc_incs.gamma.y;
}

template<typename T, typename Attribute>
Attribute computeAttributeAt(const Attribute& attr, const AttributeIncrements<Attribute>& attr_incs,
                             const Vector2<T>& offset)
{
    return attr + attr_incs.x * offset.x + attr_incs.y * offset.y;
}

enum class IncrementationMode : uint32
{
    OneInX,
    OneInY,
    OffsetInX,
    OffsetInY
};

template<IncrementationMode mode, typename Attribute>
void incrementAttributes(Attribute& attr, const AttributeIncrements<Attribute>& attr_incs, int32 offset = 1)
{
    if constexpr(mode == IncrementationMode::OneInX) {
        attr = static_cast<float>(static_cast<double>(attr) + static_cast<double>(attr_incs.x));
    }
    else if constexpr(mode == IncrementationMode::OneInY) {
        attr = static_cast<float>(static_cast<double>(attr) + static_cast<double>(attr_incs.y));
    }
    else if constexpr(mode == IncrementationMode::OffsetInX) {
        attr = static_cast<float>(static_cast<double>(attr) + static_cast<double>(attr_incs.x) * static_cast<double>(offset));
    }
    else if constexpr(mode == IncrementationMode::OffsetInY) {
        attr = static_cast<float>(static_cast<double>(attr) + static_cast<double>(attr_incs.y) * static_cast<double>(offset));
    }
    // if constexpr(mode == IncrementationMode::OneInX) {
    //     attr += attr_incs.x;
    // }
    // else if constexpr(mode == IncrementationMode::OneInY) {
    //     attr += attr_incs.y;
    // }
    // else if constexpr(mode == IncrementationMode::OffsetInX) {
    //     attr += attr_incs.x * static_cast<float>(offset);
    // }
    // else if constexpr(mode == IncrementationMode::OffsetInY) {
    //     attr += attr_incs.y * static_cast<float>(offset);
    // }
}

} // namespace MicroRenderer
