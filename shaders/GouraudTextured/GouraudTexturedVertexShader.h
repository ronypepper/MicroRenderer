//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/VertexShader.h"
#include "GouraudTexturedShaderInterface.h"

namespace MicroRenderer {

template<typename T, ShaderConfiguration t_cfg>
class GouraudTexturedVertexShader : public BaseVertexShader<T, t_cfg, GouraudTexturedShaderInterface, GouraudTexturedVertexShader>
{
public:
    using ShaderInterface_type = GouraudTexturedShaderInterface<T, t_cfg>;
    USE_SHADER_INTERFACE(ShaderInterface_type);

    static void shadeVertex_implementation(UniformData uniform, VertexData vertex)
    {
        // Transform position into (unhomogenized) screen space.
        vertex.buffer->setPosition(uniform.instance->model_screen_tf.transformPosition(vertex.source->position));

        // // Compute light intensity at vertex.
        // Vector3<T> total_intensity = static_cast<T>(0.0);
        // for (int32 i = 0; i < 3; ++i) {
        //     const auto& light = uniform.instance->point_lights[i];
        //     Vector3<T> to_light_dir = light.position - vertex.source->position;
        //     T to_light_sq_distance = to_light_dir.squaredLength();
        //     if (to_light_sq_distance > static_cast<T>(0.001)) {
        //         T to_light_distance = std::sqrt(to_light_sq_distance);
        //         T attenuation = static_cast<T>(1.0) / (static_cast<T>(0.5) + static_cast<T>(0.05) * to_light_distance + static_cast<T>(0.01) * to_light_sq_distance);
        //         Vector3<T> intensity = light.intensity * vertex.source->normal.dot(to_light_dir / to_light_distance) * attenuation;
        //         total_intensity += Vector3<T>::max(intensity, static_cast<T>(0.0));
        //     }
        // }
        // vertex.buffer->intensity = Vector3<T>::min(total_intensity + uniform.global->ambient_intensity, static_cast<T>(1.0));
    }
};

} // namespace MicroRenderer
