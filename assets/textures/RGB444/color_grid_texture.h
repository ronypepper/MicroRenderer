/* 
 * MicroRenderer texture description.
 * Texture name: color_grid_texture
 */

#pragma once
#include "MicroRenderer/Textures/TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"

using namespace MicroRenderer;

constexpr TextureAccess color_grid_texture_access = ACCESS_READONLY;
constexpr TextureInternalFormat color_grid_texture_format = FORMAT_RGB444;
constexpr int32 color_grid_texture_width = 3;
constexpr int32 color_grid_texture_height = 3;

constexpr Vector3<uint8> color_grid_texture[5] = {
    {240, 0, 15}, {15, 0, 240}, {240, 0, 15}, {0, 240, 240}, {240, 0, 0}
};
