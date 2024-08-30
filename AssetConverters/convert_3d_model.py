import sys
import os
import configparser
import pyassimp
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
    sys.exit('Error: Invalid model conversion file (not a file)!')

# Load model conversion file.
config = configparser.ConfigParser(allow_no_value=False)
if len(config.read(config_file)) == 0:
    sys.exit('Error: Model conversion file could not be read!')

# Get and verify "Shader" section of model conversion file.
if 'Shader' not in config:
    sys.exit('Invalid Configuration: "Shader" section not present!')
if 'ShaderDirectory' not in config['Shader'] or config['Shader']['ShaderDirectory'] == '':
    sys.exit('Invalid Configuration: ShaderDirectory not set!')
shader_directory = config['Shader']['ShaderDirectory']
if 'ShaderAbbreviation' not in config['Shader'] or config['Shader']['ShaderAbbreviation'] == '':
    sys.exit('Invalid Configuration: ShaderAbbreviation not set!')
shader_abbreviation = config['Shader']['ShaderAbbreviation']

# Get and verify "VertexSourceOrder" section of model conversion file.
vs_elem_names = ('Position', 'Normal', 'Tangent', 'BiTangent', 'TextureCoord_1', 'TextureCoord_2', 'TextureCoord_3',
                 'TextureCoord_4', 'VertexColor_1', 'VertexColor_2', 'VertexColor_3', 'VertexColor_4')
vs_elem_unordered = {}
if 'VertexSourceOrder' not in config:
    sys.exit('Invalid Configuration: "VertexSourceOrder" section not present!')
for name in vs_elem_names:
    if name not in config['VertexSourceOrder']:
        sys.exit('Invalid Configuration: No order set for {}!'.format(name))
    order = config['VertexSourceOrder'][name]
    if order != 'x':
        try:
            order = int(order)
            if order in vs_elem_unordered:
                sys.exit('Invalid Configuration: Two vertex source elements set to occupy order number {}!'.
                         format(order))
            vs_elem_unordered[order] = name
        except ValueError:
            sys.exit('Invalid Configuration: Order number of {} must either be an integer or "x"!'.format(name))

# Order vertex sources according to model conversion file.
vs_elem_ordered = []
for order_number in range(1, len(vs_elem_unordered) + 1):
    if order_number not in vs_elem_unordered:
        sys.exit('Error: Order numbers of vertex source elements must start with one and be coherent!')
    vs_elem_ordered.append(vs_elem_unordered[order_number])

# Template string representing the output C header file's contents (i.e. the converted model).
header_template = '''/* 
 * MicroRenderer model description.
 * Model name: %MODEL_NAME%
 * Target shader: %SHADER_DIRECTORY%
 */

#pragma once
#include "MicroRenderer/Math/ScalarTypes.h"
#include "%SHADER_DIRECTORY%/%SHADER_ABBREVIATION%ShaderInterface.h"

using namespace MicroRenderer;

constexpr int32 %MODEL_NAME%_vertex_number = %VERTEX_NUMBER%;

template<typename T>
constexpr %SHADER_ABBREVIATION%VertexSource<T> %MODEL_NAME%_vertices[%MODEL_NAME%_vertex_number] = {
    %VERTEX_ENTRIES%
};

constexpr int32 %MODEL_NAME%_triangle_number = %TRIANGLE_NUMBER%;

constexpr TriangleIndices %MODEL_NAME%_triangles[%MODEL_NAME%_triangle_number] = {
    %TRIANGLE_ENTRIES%
};

template<typename T>
constexpr typename %SHADER_ABBREVIATION%ShaderInterface<T>::ModelData %MODEL_NAME%_model = {
    %MODEL_NAME%_vertex_number, %MODEL_NAME%_triangle_number, %MODEL_NAME%_vertices<T>, %MODEL_NAME%_triangles
};
'''

# Insert information into header template.
header_template = header_template.replace('%SHADER_DIRECTORY%', shader_directory)
header_template = header_template.replace('%SHADER_ABBREVIATION%', shader_abbreviation)


def convert_model(model_file_name):
    # Construct model input file path.
    model_in_path = os.path.join(input_dir, model_file_name)
    if not os.path.isfile(model_in_path):
        return

    # Attempt to import model file with AssImp.
    try:
        with pyassimp.load(model_in_path, processing=pyassimp.postprocess.aiProcess_Triangulate) as scene:
            # Verify mesh in AssImp.
            if len(scene.meshes) == 0:
                print('Error: Assimp found no mesh in: "{}"!'.format(model_in_path))
                return
            if len(scene.meshes) > 1:
                print('WARNING: Assimp found multiple meshes in: "{}". Only one was converted.'.format(model_in_path))
            mesh = scene.meshes[0]
            if len(mesh.faces) == 0:
                print('Error: Assimp found no triangles in mesh: "{}"!'.format(model_in_path))
                return

            # Get and verify vertex sources from AssImp according to model conversion file.
            vertex_source_assimp = [mesh.vertices, mesh.normals, mesh.tangents, mesh.bitangents]
            vertex_source_assimp.extend((mesh.texturecoords[tc_idx] if len(mesh.texturecoords) > tc_idx else None
                                         for tc_idx in range(4)))
            vertex_source_assimp.extend((mesh.colors[col_idx] if len(mesh.colors) > col_idx else None
                                         for col_idx in range(4)))
            vs_elem_ordered_assimp = []
            for elem in vs_elem_ordered:
                source = vertex_source_assimp[vs_elem_names.index(elem)]
                if source is None or len(source) == 0:
                    print('Error: Assimp found no {} in mesh: "{}"!'.format(elem, model_in_path))
                    return
                vs_elem_ordered_assimp.append(source)

            # Prepare triangle indices for model header.
            triangle_indices = ''
            triangle_number = len(mesh.faces)
            for t_idx in range(triangle_number):
                new_entry = str(mesh.faces[t_idx]).replace('[', '{').replace(']', '}')
                triangle_indices += new_entry + ',\n\t'
            triangle_indices = triangle_indices[:-3]

            # Prepare vertex sources for model header.
            vertex_sources = ''
            vertex_number = len(mesh.vertices)
            for v_idx in range(vertex_number):
                new_source = ''
                for elem_name, elem_sources in zip(vs_elem_ordered, vs_elem_ordered_assimp):
                    new_entry = elem_sources[v_idx]
                    if elem_name in ('TextureCoord_1', 'TextureCoord_2', 'TextureCoord_3', 'TextureCoord_4'):
                        # Cut third value in case of texture coordinates.
                        new_entry = new_entry[:2]
                    new_entry = str(new_entry).replace('[', '{').replace(']', '}')
                    new_source += new_entry + ', '
                vertex_sources += '{' + new_source[:-2] + '},\n\t'
            vertex_sources = vertex_sources[:-3]

            # Insert information into model header.
            model_name = str(pathlib.Path(model_file_name).with_suffix(''))
            converted_model = header_template.replace('%MODEL_NAME%', model_name)
            converted_model = converted_model.replace('%TRIANGLE_NUMBER%', str(triangle_number))
            converted_model = converted_model.replace('%VERTEX_NUMBER%', str(vertex_number))
            converted_model = converted_model.replace('%TRIANGLE_ENTRIES%', triangle_indices)
            converted_model = converted_model.replace('%VERTEX_ENTRIES%', vertex_sources)

            # Write header file in output directory.
            model_out_path = os.path.join(output_dir, model_name + '.h')
            with open(model_out_path, 'w') as out_file:
                out_file.write(converted_model)
                print('SUCCESS: Converted model from file: "{}" to: "{}".'.format(model_in_path, model_out_path))
    except pyassimp.AssimpError:
        print('Error: Assimp failed to import file: "{}"!'.format(model_in_path))


# Convert all files in input directory.
for file_name in os.listdir(input_dir):
    convert_model(file_name)
