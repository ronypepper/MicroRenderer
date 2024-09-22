/* 
 * MicroRenderer texture description.
 * Texture name: color_grid_texture
 */

#pragma once
#include "MicroRenderer/Core/Textures/TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"

using namespace MicroRenderer;

constexpr TextureAccess color_grid_texture_access = ACCESS_READONLY;
constexpr TextureInternalFormat color_grid_texture_format = FORMAT_RGB888;
constexpr int32 color_grid_texture_width = 3;
constexpr int32 color_grid_texture_height = 3;

constexpr uint8 color_grid_texture[27] = {
    255, 0, 0, 0, 0, 255, 0, 255, 0, 0, 255, 0, 255, 0, 0, 0, 0, 255, 0, 0, 
	255, 0, 255, 0, 255, 0, 0
};
