//
// Created by pusdo on 29/07/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/ShaderInterface.h"
#include "MicroRenderer/Math/Vector3.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg, template <typename, ShaderConfiguration> class Interface,
         template <typename, ShaderConfiguration> class Derived>
class BaseFragmentShader
{
public:
    using ShaderInterface_type = Interface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    template<IncrementationMode mode>
    static void interpolateAttributes(UniformData uniform, TriangleBuffer* triangle, int32 offset)
    {
        Derived<T, t_cfg>::template interpolateAttributes_implementation<mode>(uniform, triangle, offset);
    }

    static ShaderOutput computeColor(UniformData uniform, TriangleBuffer* triangle)
    {
        return Derived<T, t_cfg>::computeColor_implementation(uniform, triangle);
    }
};

} // namespace MicroRenderer
