import sys
import os
import configparser
from PIL import Image
from PIL import UnidentifiedImageError
import pathlib

# Extract cmd line arguments.
input_dir = sys.argv[1]
output_dir = sys.argv[2]
config_file = sys.argv[3]

# Check cmd line arguments.
if not os.path.isdir(input_dir):
    sys.exit('Error: Invalid input directory!')
if not os.path.isdir(output_dir):
    sys.exit('Error: Invalid output directory!')
if not os.path.isfile(config_file):
    sys.exit('Error: Invalid texture conversion file (not a file)!')

# Load texture conversion file.
config = configparser.ConfigParser(allow_no_value=False)
if len(config.read(config_file)) == 0:
    sys.exit('Error: Texture conversion file could not be read!')

# Get and verify "Settings" section of texture conversion file.
if 'Settings' not in config:
    sys.exit('Invalid Configuration: "Settings" section not present!')

# Get and verify "ReadOnlyAccess" entry of texture conversion file.
if 'ReadOnlyAccess' not in config['Settings']:
    sys.exit('Invalid Configuration: ReadOnlyAccess not set!')
try:
    read_only_access = config['Settings'].getboolean('ReadOnlyAccess')
except ValueError:
    sys.exit('Invalid Configuration: ReadOnlyAccess must be true or false!')

# Get and verify "Dimension" entry of texture conversion file.
if 'Dimension' not in config['Settings']:
    sys.exit('Invalid Configuration: Dimension not set!')
try:
    dimension = config['Settings'].getint('Dimension')
    if dimension < 1 or dimension > 4:
        sys.exit('Invalid Configuration: Dimension must be between 1 and 4!')
except ValueError:
    sys.exit('Invalid Configuration: Dimension must be between 1 and 4!')

# Get and verify "BitDepthRed", "BitDepthGreen", "BitDepthBlue", "BitDepthAlpha" entries of texture conversion file.
bit_depths = []
for bit_depth_name in ('BitDepthRed', 'BitDepthGreen', 'BitDepthBlue', 'BitDepthAlpha'):
    if bit_depth_name not in config['Settings']:
        sys.exit('Invalid Configuration: {} not set!'.format(bit_depth_name))
    try:
        bit_depths.append(config['Settings'].getint(bit_depth_name))
        if bit_depths[-1] < 1 or bit_depths[-1] > 8:
            sys.exit('Invalid Configuration: {} must be between 1 and 8!'.format(bit_depth_name))
    except ValueError:
        sys.exit('Invalid Configuration: {} must be between 1 and 8!'.format(bit_depth_name))
bit_depths = bit_depths[:dimension]

# Template string representing the output C header file's contents (i.e. the converted texture).
header_template = '''/* 
 * MicroRenderer texture description.
 * Texture name: %TEXTURE_NAME%
 */

#pragma once
#include "MicroRenderer/Core/Textures/TextureConfiguration.h"
#include "MicroRenderer/Math/ScalarTypes.h"

using namespace MicroRenderer;

constexpr TextureAccess %TEXTURE_NAME%_access = %ACCESS%;
constexpr TextureInternalFormat %TEXTURE_NAME%_format = %FORMAT%;
constexpr int32 %TEXTURE_NAME%_width = %WIDTH%;
constexpr int32 %TEXTURE_NAME%_height = %HEIGHT%;

%ACCESS_MODIFIER% uint8 %TEXTURE_NAME%[%NUM_PIXEL_DATA_BYTES%] = {
    %PIXEL_DATA%
};
'''

# Insert access information into header template.
if read_only_access:
    header_template = header_template.replace('%ACCESS%', 'ACCESS_READONLY')
    header_template = header_template.replace('%ACCESS_MODIFIER%', 'constexpr')
else:
    header_template = header_template.replace('%ACCESS%', 'ACCESS_READWRITE')
    header_template = header_template.replace('%ACCESS_MODIFIER%', 'inline')

# Verify format and insert format information into header template.
if dimension == 3 and bit_depths[0] == 8 and bit_depths[1] == 8 and bit_depths[2] == 8:
    header_template = header_template.replace('%FORMAT%', 'FORMAT_RGB888')
elif dimension == 3 and bit_depths[0] == 5 and bit_depths[1] == 6 and bit_depths[2] == 5:
    header_template = header_template.replace('%FORMAT%', 'FORMAT_RGB565')
elif dimension == 3 and bit_depths[0] == 4 and bit_depths[1] == 4 and bit_depths[2] == 4:
    header_template = header_template.replace('%FORMAT%', 'FORMAT_RGB444')
elif dimension == 4 and bit_depths[0] == 4 and bit_depths[1] == 4 and bit_depths[2] == 4 and bit_depths[3] == 4:
    header_template = header_template.replace('%FORMAT%', 'FORMAT_RGBA4444')
elif dimension == 1 and bit_depths[0] == 8:
    header_template = header_template.replace('%FORMAT%', 'FORMAT_R8')
else:
    sys.exit('Invalid Configuration: Dimension/BitDepth(s) combination not supported!')


def convert_texture(texture_file_name):
    # Construct texture input file path.
    texture_in_path = os.path.join(input_dir, texture_file_name)
    if not os.path.isfile(texture_in_path):
        return

    # Attempt to import texture file with Pillow.
    try:
        img = Image.open(texture_in_path)

        # Verify image width/height.
        img_width, img_height = img.size
        if img_width < 1 or img_height < 1:
            print('Error: Image has zero pixels: "{}"!'.format(texture_in_path))
            return

        # Verify image dimension compatibility.
        if img.mode != 'RGB' and img.mode != 'RGBA':
            print('Error: Input image must have RGB or RGBA mode: "{}"!'.format(texture_in_path))
            return
        if dimension == 4 and img.mode != 'RGBA':
            print('Error: Input image must have RGBA mode for Dimension = 4: "{}"!'.format(texture_in_path))
            return

        # Prepare pixel data for texture header.
        pixel_data = ''
        uint8_buffer = 0
        next_bit_in_buffer_mask = 1 << 7
        num_written_uint8s = 0
        for y in range(img_height):
            for x in range(img_width):
                # Get pixel.
                pixel = img.getpixel((x, y))
                for idx, depth in enumerate(bit_depths):
                    # Get value of pixel channel adjusted to its desired bit depth.
                    value = (pixel[idx] & 0xff) >> (8 - depth)

                    # Iterate over bits in value, writing them to 8-bit sized output buffer.
                    # When buffer is full, insert string into pixel_data for header and reset buffer.
                    remaining_bits_in_value = depth
                    while remaining_bits_in_value > 0:
                        # Get next bit.
                        remaining_bits_in_value -= 1
                        next_bit_in_value_mask = 1 << remaining_bits_in_value
                        next_bit = value & next_bit_in_value_mask

                        # Write bit into buffer.
                        if next_bit != 0:
                            uint8_buffer |= next_bit_in_buffer_mask
                        next_bit_in_buffer_mask = next_bit_in_buffer_mask >> 1

                        # Append to string for header and reset buffer when full.
                        if next_bit_in_buffer_mask == 0:
                            pixel_data += str(uint8_buffer) + ', '
                            num_written_uint8s += 1
                            if num_written_uint8s % 20 == 0:
                                pixel_data += '\n\t'
                            uint8_buffer = 0
                            next_bit_in_buffer_mask = 1 << 7
        # Write partially filled last buffer to string for header.
        if next_bit_in_buffer_mask < 1 << 7:
            pixel_data += str(uint8_buffer)
            num_written_uint8s += 1
        else:
            if num_written_uint8s % 20 == 0:
                pixel_data = pixel_data[:-4]
            else:
                pixel_data = pixel_data[:-2]

        # Insert information into texture header.
        texture_name = str(pathlib.Path(texture_file_name).with_suffix(''))
        converted_texture = header_template.replace('%TEXTURE_NAME%', texture_name)
        converted_texture = converted_texture.replace('%WIDTH%', str(img_width))
        converted_texture = converted_texture.replace('%HEIGHT%', str(img_height))
        converted_texture = converted_texture.replace('%NUM_PIXEL_DATA_BYTES%', str(num_written_uint8s))
        converted_texture = converted_texture.replace('%PIXEL_DATA%', pixel_data)

        # Write header file in output directory.
        texture_out_path = os.path.join(output_dir, texture_name + '.h')
        with open(texture_out_path, 'w') as out_file:
            out_file.write(converted_texture)
            print('SUCCESS: Converted texture from file: "{}" to: "{}".'.format(texture_in_path, texture_out_path))
    except (FileNotFoundError, UnidentifiedImageError, ValueError, TypeError):
        print('Error: Pillow failed to read image: "{}"!'.format(texture_in_path))


# Convert all files in input directory.
for file_name in os.listdir(input_dir):
    convert_texture(file_name)
