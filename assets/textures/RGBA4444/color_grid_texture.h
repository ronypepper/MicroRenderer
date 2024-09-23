/* 
 * MicroRenderer texture description.
 * Texture name: color_grid_texture
 */

#pragma once
#include "MicroRenderer/Textures/TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"

using namespace MicroRenderer;

constexpr TextureAccess color_grid_texture_access = ACCESS_READONLY;
constexpr TextureInternalFormat color_grid_texture_format = FORMAT_RGBA4444;
constexpr int32 color_grid_texture_width = 3;
constexpr int32 color_grid_texture_height = 3;

constexpr uint16 color_grid_texture[9] = {
    61455, 255, 3855, 3855, 61455, 255, 255, 3855, 61455
};
