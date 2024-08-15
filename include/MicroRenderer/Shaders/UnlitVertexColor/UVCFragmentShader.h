//
// Created by pusdo on 04/08/2024.
//

#pragma once
#include "MicroRenderer/Core/Shading/FragmentShader.h"
#include "MicroRenderer/Shaders/UnlitVertexColor/UVCShaderInterface.h"

namespace MicroRenderer {

template<typename T>
class UVCFragmentShader : public BaseFragmentShader<T, UVCShaderInterface, UVCFragmentShader>
{
public:
    USE_SHADER_INTERFACE(UVCShaderInterface<T>);

    static void interpolateTo_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle,
                                             VertexData vertex, const Vector2<T>& offset, int32 x, int32 y)
    {
        // Vertex color.
        fragment->color = computeAttributeAt(vertex.source->color, triangle.buffer->color_incs, offset);

        // Texture.
        fragment->uv = computeAttributeAt(vertex.source->uv_coordinates, triangle.buffer->uv_incs, offset);
    }
    static void interpolateRight_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        // Vertex color.
        incrementAttributeRight(fragment->color, triangle.buffer->color_incs);

        // Texture.
        incrementAttributeRight(fragment->uv, triangle.buffer->uv_incs);
    }
    static ShaderOutput computeColor_implementation(UniformData uniform, Fragment* fragment, TriangleData triangle)
    {
        // Vertex color.
        return fragment->color;

        // Texture.
        return uniform.global->tex_color.samplePixelAt(fragment->uv);
    }
};

} // namespace MicroRenderer
