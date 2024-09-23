//
// Created by pusdo on 22/07/2024.
//

#pragma once
#include "MicroRenderer/Shading/ShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg, template <typename, ShaderConfiguration> class Interface,
         template <typename, ShaderConfiguration> class Derived>
class BaseVertexShader
{
public:
    using ShaderInterface_type = Interface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void shadeVertex(UniformData uniform, VertexData vertex)
    {
        Derived<T, t_cfg>::shadeVertex_implementation(uniform, vertex);
    }
};

} // namespace MicroRenderer
