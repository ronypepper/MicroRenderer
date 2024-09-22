//
// Created by pusdo on 01/09/2024.
//

#pragma once
#include "MicroRenderer/Core/Textures/TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"

namespace MicroRenderer {

enum ProjectionType : uint32
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
    NUM_PROJECTION_TYPES
};

enum CullingMode : uint32
{
    CULL_AT_SCREEN_BORDER,
    NO_CULLING,
    NUM_CULLING_MODES
};

enum ClippingMode : uint32
{
    CLIP_AT_NEAR_PLANE,
    NO_CLIPPING,
    NUM_CLIPPING_MODES
};

enum DepthTestMode : uint32
{
    DEPTH_TEST_ENABLED,
    DEPTH_TEST_DISABLED,
    NUM_DEPTH_TEST_MODES
};

enum ShadingMode : uint32
{
    SHADING_ENABLED,
    SHADING_DISABLED,
    NUM_SHADING_MODES
};

struct ShaderOutput
{
    TextureInternalFormat format;
    TextureSwizzle swizzle;
    TextureExternalType type;
};

struct ShaderConfiguration
{
    ProjectionType projection;
    CullingMode culling;
    ClippingMode clipping;
    DepthTestMode depth_test;
    ShadingMode shading;
    ShaderOutput output;
};

} // namespace MicroRenderer
