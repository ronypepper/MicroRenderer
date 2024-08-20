//
// Created by pusdo on 29/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, template <typename> class Interface, template <typename> class Derived>
class BaseFragmentShader
{
public:
    USE_SHADER_INTERFACE(Interface<T>);

    template<IncrementationMode mode>
    static void interpolateAttributes(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        Derived<T>::interpolateAttributes_implementation<mode>(uniform, triangle, offset);
    }

    static ShaderOutput computeColor(UniformData uniform, TriangleBuffer* triangle)
    {
        return Derived<T>::computeColor_implementation(uniform, triangle);
    }
};

} // namespace MicroRenderer
