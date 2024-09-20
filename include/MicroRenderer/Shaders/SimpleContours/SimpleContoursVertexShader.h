//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "MicroRenderer/Shaders/SimpleContours/SimpleContoursShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class SimpleContoursVertexShader : public BaseVertexShader<T, t_cfg, SimpleContoursShaderInterface, SimpleContoursVertexShader>
{
public:
    using ShaderInterface_type = SimpleContoursShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void shadeVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // Transform position into (unhomogenized) screen space.
        vertex.buffer->setPosition(uniform.instance->model_screen_tf.transformPosition(vertex.source->position));
    }
};

} // namespace MicroRenderer
