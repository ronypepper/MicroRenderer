//
// Created by pusdo on 13/08/2024.
//

/*
 * This demo requires SDL2 to be installed, available under the zlib license.
 *
 * The demo can be switched to two modes under --- Demo Configuration --- below:
 * 1. MODE_FREEFLY: Allows freefly navigation by 'WASDQE' for movement and the arrow keys for rotation.
 *					The image can be saved as .ppm files by pressing the 'P' key.
 *					Color/Depth information display can be toggled by pressing the 'Space' key.
 * 2. MODE_CAPTURE_STATS: Captures a number of frames and then prints performance metrics to console.
 *
 * Two shaders can be selected under --- Demo Configuration --- below:
 * 1. SHADER_SIMPLECONTOURS: see shaders directory.
 * 2. SHADER_UNLITTEXTURED: see shaders directory.
 */

#include "SDL.h"
#include <chrono>

#include "MicroRenderer/MicroRenderer.h"

using namespace MicroRenderer;

#define SHADER_SIMPLECONTOURS 0
#define SHADER_UNLITTEXTURED 1

#define MODE_FREEFLY 0
#define MODE_CAPTURE_STATS 1

// ------------------ Demo configuration --------------------- //

#define DEMO_MODE MODE_CAPTURE_STATS
#define USED_SHADER SHADER_UNLITTEXTURED

// ------------------ Demo configuration --------------------- //

// ------------------ Renderer configuration --------------------- //

using DataType = double;
constexpr int32 window_width = 1000;
constexpr int32 window_height = 1000;
constexpr ShaderConfiguration my_shader_cfg = {
	PERSPECTIVE, CULL_AT_SCREEN_BORDER, CLIP_AT_NEAR_PLANE, DEPTH_TEST_ENABLED, SHADING_ENABLED,
	{FORMAT_RGB888, SWIZZLE_NONE, TYPE_INTEGER}
};
constexpr RendererConfiguration my_renderer_cfg = {SCANLINE, CLOCKWISE, my_shader_cfg};

// ------------------ Renderer configuration --------------------- //

// ------------------ Shader, Model and Texture includes --------------------- //

#if USED_SHADER == SHADER_SIMPLECONTOURS
#include "SimpleContours/SimpleContoursShaderProgram.h"
#include "models/SimpleContours/cube.h"
#include "models/SimpleContours/tu_vienna_logo.h"
#elif USED_SHADER == SHADER_UNLITTEXTURED
#include "UnlitTextured/UnlitTexturedShaderProgram.h"
#include "models/UnlitTextured/cube.h"
#include "models/UnlitTextured/plane.h"
#include "textures/RGB888/color_grid_texture.h"
#include "textures/RGB888/tu_logo_texture.h"
#endif

// ------------------ Shader, Model and Texture includes --------------------- //

// ------------------ Renderer globals --------------------- //

// Renderer object.
#if USED_SHADER == SHADER_SIMPLECONTOURS
using MyRenderer = Renderer<DataType, my_renderer_cfg, SimpleContoursShaderProgram>;
#elif USED_SHADER == SHADER_UNLITTEXTURED
using MyRenderer = Renderer<DataType, my_renderer_cfg, UnlitTexturedShaderProgram>;
#endif
using MyShaderProgram = MyRenderer::ShaderProgram_type;
MyRenderer my_renderer;

// Buffers.
#if USED_SHADER == SHADER_SIMPLECONTOURS
constexpr uint16 num_vertex_buffers = std::max(cube_vertex_number, tu_vienna_logo_vertex_number);
#elif USED_SHADER == SHADER_UNLITTEXTURED
constexpr uint16 num_vertex_buffers = std::max(cube_vertex_number, plane_vertex_number);
#endif
MyRenderer::VertexBuffer vertex_buffer[num_vertex_buffers];
constexpr uint16 num_rasterization_structs = 400;
MyRenderer::RasterizationBuffer rasterization_buffers[num_rasterization_structs];
MyRenderer::RasterizationOrder rasterization_order[num_rasterization_structs];

// Frame/depthbuffer.
void* framebuffer_address = nullptr;
DataType depthbuffer_address[window_width * window_height];

// Screen-projection transform.
constexpr float aspect_ratio = static_cast<float>(window_height) / static_cast<float>(window_width);
Matrix4<DataType> screen_proj_tf;

// View state.
#if USED_SHADER == SHADER_SIMPLECONTOURS
Vector3<DataType> view_position = {2.84572, -2.857554, -4.254630};
Vector3<DataType> view_orientation = {-29.76, -37.5, 0.};
#elif USED_SHADER == SHADER_UNLITTEXTURED
Vector3<DataType> view_position = {4.872785, -3.176531, -7.266366};
Vector3<DataType> view_orientation = {-20.82, -31.38, 0.};
#endif
constexpr DataType view_translation_speed = 2.;
constexpr DataType view_rotation_speed = 30.;
Vector3<DataType> view_fwd_dir, view_up_dir, view_right_dir;

// ------------------ Renderer globals --------------------- //

// ------------------ Shader globals --------------------- //

#if USED_SHADER == SHADER_SIMPLECONTOURS
// Triangle normals.
Vector3<DataType> cube_tri_normals[cube_triangle_number];
Vector3<DataType> tu_vienna_logo_tri_normals[tu_vienna_logo_triangle_number];

// Lighting.
Vector3<DataType> towards_sun_dir_world_space = Vector3<DataType>(2.0, 0.3, 0.7).getNormalized();

// Global data.
MyRenderer::GlobalData global_data = {};

// Instance data.
constexpr uint16 num_instances = 3;
MyRenderer::InstanceData instances[num_instances] = {
	{2, {}, tu_vienna_logo_tri_normals, {}, {0.0, 0.0, 255.0}},
	{0, {}, cube_tri_normals, {}, {255.0, 0.0, 0.0}},
	{0, {}, cube_tri_normals, {}, {0.0, 255.0, 0.0}}
};

// Models.
constexpr MyRenderer::ModelData models[] = {
	cube_model<DataType, my_shader_cfg>,
	cube_model<DataType, my_shader_cfg>,
	tu_vienna_logo_model<DataType, my_shader_cfg>
};
#elif USED_SHADER == SHADER_UNLITTEXTURED
// // Lighting.
// PointLight<DataType> world_point_lights[3] = {
// 	{{2.3, -0.1, 0.8}, {3.0, 0.1, 0.1}},
// 	{{-1.5, -1.3, -0.7}, {0.1, 3.0, 0.1}},
// 	{{0.0, -0.3, 0.5}, {0.1, 0.1, 3.0}}
// };

// Global data.
MyRenderer::GlobalData global_data = {};

// Instance data.
constexpr uint16 num_instances = 3;
MyRenderer::InstanceData instances[num_instances] = {
    {0 ,{1.0}, {color_grid_texture, color_grid_texture_width, color_grid_texture_height}},
    {1 ,{1.0}, {tu_logo_texture, tu_logo_texture_width, tu_logo_texture_height}},
    {1 ,{1.0}, {tu_logo_texture, tu_logo_texture_width, tu_logo_texture_height}}
};

// Models.
constexpr MyRenderer::ModelData models[] = {
	cube_model<DataType, my_shader_cfg>,
	plane_model<DataType, my_shader_cfg>,
	plane_model<DataType, my_shader_cfg>
};
#endif

// Scene movement.
DataType objects_y_rot = 180.0;
DataType cubes_rot_speed = 15.0;

// ------------------ Shader globals --------------------- //

// ------------------ Rendering functions --------------------- //

void initializeRenderer()
{
#if USED_SHADER == SHADER_SIMPLECONTOURS
	// Preprocess triangle normals.
	preprocessTriangleNormals<DataType, MyShaderProgram>(&cube_model<DataType, my_shader_cfg>, cube_tri_normals);
	preprocessTriangleNormals<DataType, MyShaderProgram>(&tu_vienna_logo_model<DataType, my_shader_cfg>, tu_vienna_logo_tri_normals);
#endif

	// Set resolution.
	my_renderer.setResolution(window_width, window_height);

	// Compute screen-projection transform.
	screen_proj_tf = Transform::viewport<DataType>(window_width, window_height, false, false);
	if constexpr (my_shader_cfg.projection == PERSPECTIVE) {
		screen_proj_tf *= Transform::perspectiveProjection<DataType>(-0.02, 0.02, -0.02 * aspect_ratio,
		                                                             0.02 * aspect_ratio, 0.1, 15.);
		my_renderer.setNearPlane(DataType(0.1));
	}
	else {
		screen_proj_tf *= Transform::orthographicProjection<DataType>(-1., 1., -1., 1., 0.1, 15.);
	}

	// Set rendering data.
	my_renderer.setModels(models);
	my_renderer.setInstances(instances, num_instances);
	my_renderer.setGlobalData(&global_data);
	my_renderer.setVertexBuffers(vertex_buffer);
	my_renderer.setRasterizationBuffers(rasterization_buffers, rasterization_order, num_rasterization_structs);
}

void updateRenderer(DataType delta_time)
{
	// Compute camera directions and transform.
	Matrix3<DataType> view_rotation = Transform::rotationEuler(view_orientation).getMatrix3();
	view_fwd_dir = view_rotation * Vector3<DataType>(0., 0., 1.);
	view_up_dir = view_rotation * Vector3<DataType>(0., 1., 0.);
	view_right_dir = view_rotation * Vector3<DataType>(1., 0., 0.);
	Matrix4<DataType> view_tf = Transform::camera<DataType>(view_position, view_fwd_dir, view_up_dir);

	// Compute screen-projection-camera transform.
	const Matrix4<DataType> screen_proj_view_tf = screen_proj_tf * view_tf;

	// Rotate cubes.
#if DEMO_MODE == MODE_FREEFLY
	objects_y_rot += cubes_rot_speed * delta_time;
#elif DEMO_MODE == MODE_CAPTURE_STATS
	objects_y_rot += 0.1;
#endif

#if USED_SHADER == SHADER_SIMPLECONTOURS
	// TU Vienna Logo transform.
    Matrix4<DataType> tu_logo_model_tf = Transform::translation<DataType>(Vector3<DataType>(0., 0., 0.2));
    tu_logo_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., objects_y_rot, 0.));
    tu_logo_model_tf *= Transform::scale<DataType>(Vector3<DataType>(0.3));
    instances[0].model_screen_tf = screen_proj_view_tf * tu_logo_model_tf;
    Vector3<DataType> tu_logo_sun_dir = tu_logo_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[0].towards_sun_dir_model_space = tu_logo_sun_dir.getNormalized();
    // Cube 1 transform.
    Matrix4<DataType> cube_1_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1.1, 0.2, -0.1));
    cube_1_model_tf *= Transform::rotationEuler(Vector3<DataType>(0., objects_y_rot, 0.));
    cube_1_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[1].model_screen_tf = screen_proj_view_tf * cube_1_model_tf;
    Vector3<DataType> cube_1_sun_dir = cube_1_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[1].towards_sun_dir_model_space = cube_1_sun_dir.getNormalized();
    // Cube 2 transform.
    Matrix4<DataType> cube_2_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1.3, 0.4, -0.2));
    cube_2_model_tf *= Transform::rotationEuler(Vector3<DataType>(20., 10. + objects_y_rot, 35.));
    cube_2_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[2].model_screen_tf = screen_proj_view_tf * cube_2_model_tf;
    Vector3<DataType> cube_2_sun_dir = cube_2_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[2].towards_sun_dir_model_space = cube_2_sun_dir.getNormalized();
#elif USED_SHADER == SHADER_UNLITTEXTURED
    // Cube transform.
    Matrix4<DataType> cube_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1., 0., 0.2));
    cube_model_tf *= Transform::rotationEuler(Vector3<DataType>(0., objects_y_rot, 0.));
    cube_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[0].model_screen_tf = screen_proj_view_tf * cube_model_tf;
    // for (int32 i = 0; i < 3; ++ i) {
    //     instances[0].point_lights[i] = {
    //         world_point_lights[i].intensity,
    //         world_point_lights[i].position
    //     	};
    // }
    // Plane 1 transform.
    DataType plane_scale_mod = static_cast<DataType>(tu_logo_texture_width) / static_cast<DataType>(tu_logo_texture_height);
    Matrix4<DataType> plane_1_model_tf = Transform::translation<DataType>(Vector3<DataType>(1., 0., 0.2));
    plane_1_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., 180. + objects_y_rot, 180.));
    plane_1_model_tf *= Transform::scale<DataType>(Vector3<DataType>(plane_scale_mod * 0.5, 0.5, 0.5));
    instances[1].model_screen_tf = screen_proj_view_tf * plane_1_model_tf;
    // Plane 2 transform.
    Matrix4<DataType> plane_2_model_tf = Transform::translation<DataType>(Vector3<DataType>(1., 0., 0.2));
    plane_2_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., objects_y_rot, 180.));
    plane_2_model_tf *= Transform::scale<DataType>(Vector3<DataType>(plane_scale_mod * 0.5, 0.5, 0.5));
    instances[2].model_screen_tf = screen_proj_view_tf * plane_2_model_tf;
#endif
}

void clearRenderer()
{
	// Set start buffer addresses.
	my_renderer.setFramebuffer(framebuffer_address);
	my_renderer.setDepthbuffer(depthbuffer_address);

	// Override buffer resolutions.
	my_renderer.getFramebuffer().setResolution(window_width, window_height);
	my_renderer.getDepthbuffer().setResolution(window_width, window_height);

	// Clear buffers.
	my_renderer.getFramebuffer().clearBuffer({0});
	my_renderer.getDepthbuffer().clearBuffer(0.);

	// Reset resolutions.
	my_renderer.setResolution(window_width, window_height);
}

void drawRenderer()
{
	my_renderer.render();

	for (int32 y = 0; y < window_height; ++y) {
		void* framebuffer_scanline = static_cast<MyRenderer::Framebuffer::InternalType*>(framebuffer_address) + y * window_width;
		void* depthbuffer_scanline = static_cast<MyRenderer::Depthbuffer::InternalType*>(depthbuffer_address) + y * window_width;
		my_renderer.setFramebuffer(framebuffer_scanline);
		my_renderer.setDepthbuffer(depthbuffer_scanline);
		my_renderer.renderNextScanline();
	}
}

// ------------------ Rendering functions --------------------- //

// ------------------ SDL Demo --------------------- //

// SDL pointers.
SDL_Window* sdl_window = nullptr;
SDL_Renderer* sdl_renderer = nullptr;
SDL_Texture* sdl_frame_texture = nullptr;

// Input mode selection.
bool show_depth = false;
bool show_depth_helper = true;
bool save_image = false;
bool save_image_helper = true;

// Window scale.
constexpr int32 window_scale_x = 1;
constexpr int32 window_scale_y = 1;

// Timing.
std::chrono::time_point<std::chrono::steady_clock> last_frame_time;

// Stats.
volatile uint32 frame_time = 0;
constexpr uint32 capture_num_frames = 3600;
uint32 min_frame_time = 0xFFFFFFFF;
uint32 max_frame_time = 0;
uint32 sum_frame_time = 0;
uint32 num_frames = 0;
bool stats_printed = false;

void handleSDLError(int error_code)
{
    if (error_code) {
        const char* error_msg = SDL_GetError();
        printf(error_msg);
        exit(1);
    }
}

void init()
{
	// Initialize SDL.
	handleSDLError(SDL_Init(SDL_INIT_VIDEO));
	handleSDLError(SDL_CreateWindowAndRenderer(window_width * window_scale_x, window_height * window_scale_y, 0,
											   &sdl_window, &sdl_renderer));

	// Create SDL texture.
	sdl_frame_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
										  window_width, window_height);
	handleSDLError(sdl_frame_texture == nullptr);

	last_frame_time = std::chrono::high_resolution_clock::now();

	initializeRenderer();

#if DEMO_MODE == MODE_CAPTURE_STATS
	printf("CAPTURING STATS ...");
#endif
}

void update()
{
	// Compute delta time.
	auto current_time = std::chrono::high_resolution_clock::now();
    uint32 delta_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_frame_time).count();
    DataType delta_time = static_cast<DataType>(delta_microseconds) / static_cast<DataType>(1000000.0);
    last_frame_time = current_time;

    // Poll SDL events.
    SDL_PumpEvents();

    // Quit application with window "x" button.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            SDL_Quit();
            exit(0);
        }
    }

#if DEMO_MODE == MODE_FREEFLY
    // Input.
    const uint8* keyboard_states = SDL_GetKeyboardState(nullptr);

    if (keyboard_states[SDL_SCANCODE_SPACE]) {
        if (show_depth_helper) {
            show_depth = !show_depth;
        }
        show_depth_helper = false;
    }
    else {
        show_depth_helper = true;
    }

	if (keyboard_states[SDL_SCANCODE_P]) {
		if (save_image_helper) {
			save_image = !save_image;
		}
		save_image_helper = false;
	}
	else {
		save_image_helper = true;
	}

    auto translation_speed = view_translation_speed;
    auto rotation_speed = view_rotation_speed;
    if (keyboard_states[SDL_SCANCODE_LSHIFT]) {
        translation_speed *= 0.25;
        rotation_speed *= 0.25;
    }
    if (keyboard_states[SDL_SCANCODE_A]) {
        view_position -= view_right_dir * translation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_D]) {
        view_position += view_right_dir * translation_speed * delta_time;
    }
    if (keyboard_states[SDL_SCANCODE_Q]) {
        view_position += view_up_dir * translation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_E]) {
        view_position -= view_up_dir * translation_speed * delta_time;
    }
    if (keyboard_states[SDL_SCANCODE_W]) {
        view_position += view_fwd_dir * translation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_S]) {
        view_position -= view_fwd_dir * translation_speed * delta_time;
    }
    if (keyboard_states[SDL_SCANCODE_LEFT]) {
        view_orientation.y -= rotation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_RIGHT]) {
        view_orientation.y += rotation_speed * delta_time;
    }
    if (keyboard_states[SDL_SCANCODE_UP]) {
        view_orientation.x += rotation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_DOWN]) {
        view_orientation.x -= rotation_speed * delta_time;
    }
    if (keyboard_states[SDL_SCANCODE_COMMA]) {
        view_orientation.z += rotation_speed * delta_time;
    }
    else if (keyboard_states[SDL_SCANCODE_PERIOD]) {
        view_orientation.z -= rotation_speed * delta_time;
    }
#endif

	// Update call.
	auto start = std::chrono::high_resolution_clock::now();
    updateRenderer(delta_time);
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	frame_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
}

void draw()
{
#if DEMO_MODE == MODE_FREEFLY
	// Clear call.
	clearRenderer();

	// Draw call.
	auto start = std::chrono::high_resolution_clock::now();
	drawRenderer();
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	frame_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

	// Print FPS info to console.
	printf("\nFPS: %f", 1000000.0 / static_cast<double>(frame_time));

	// Show depth.
	if (show_depth) {
		auto* pixel_pointer = static_cast<uint8*>(framebuffer_address);
		for (int32 i = 0; i < window_width * window_height; ++i) {
			auto depth = static_cast<uint8>(depthbuffer_address[i] * 255.f);
			*pixel_pointer++ = depth;
			*pixel_pointer++ = depth;
			*pixel_pointer++ = depth;
		}
	}
#elif DEMO_MODE == MODE_CAPTURE_STATS
	if (num_frames < capture_num_frames) {
		// Clear call.
		clearRenderer();

		// Draw call.
		auto start = std::chrono::high_resolution_clock::now();
		drawRenderer();
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		frame_time += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

		// Update rendering time stats.
		sum_frame_time += frame_time;
		if (frame_time < min_frame_time)
			min_frame_time = frame_time;
		if (frame_time > max_frame_time)
			max_frame_time = frame_time;

		// Print remaining frames info to console.
		++num_frames;
		printf("\nFRAMES: %i/%i", num_frames, capture_num_frames);
	}
	else if (!stats_printed) {
		// Print rendering time stats info to console.
		printf("\n\nSTATS CAPTURED.");
		const double avg_time = static_cast<double>(sum_frame_time) / static_cast<double>(capture_num_frames) / 1000.0;
		printf("\nAvg [ms]: %f", avg_time);
		const double min_time = static_cast<double>(min_frame_time) / 1000.0;
		printf("\nMin [ms]: %f", min_time);
		const double max_time = static_cast<double>(max_frame_time) / 1000.0;
		printf("\nMax [ms]: %f", max_time);
		printf("\nAvg [fps]: %f", 1000.0 / avg_time);
		printf("\nMin [fps]: %f", 1000.0 / max_time);
		printf("\nMax [fps]: %f", 1000.0 / min_time);
		stats_printed = true;
	}
#endif
}

int main(int argc, char *argv[])
{
    init();

	// Update and draw loop.
    while (true) {
        // Lock SDL texture.
        int sdl_framebuffer_pitch = 0;
        handleSDLError(SDL_LockTexture(sdl_frame_texture, nullptr, &framebuffer_address, &sdl_framebuffer_pitch));

    	update();
    	draw();

    	// Print viewer position and orientation to console.
        //printf("\nTRA: X: %f, Y: %f, Z: %f\n", view_position.x, view_position.y, view_position.z);
    	//printf("ROT: X: %f, Y: %f, Z: %f\n", view_orientation.x, view_orientation.y, view_orientation.z);

        // Save framebuffer and depthbuffer to image.
        if (save_image) {
        	save_image = false;

            // Determine file number.
            int32 next_file_number = 1;
            const std::string base_name = show_depth ? "depth_render_" : "frame_render_";
            while (next_file_number <= 10000) {
                std::ifstream file(base_name + std::to_string(next_file_number) + std::string(".ppm"));
                if (!file.good()) {
                    break;
                }
                ++next_file_number;
            }

            // Override frame/depthbuffer resolution and set addresses in case of Scanline Rendering.
            my_renderer.getFramebuffer().setResolution(window_width, window_height);
            my_renderer.getDepthbuffer().setResolution(window_width, window_height);
            my_renderer.setFramebuffer(framebuffer_address);
            my_renderer.setDepthbuffer(depthbuffer_address);

            // Save image.
            const std::string file_name = base_name + std::to_string(next_file_number);
            if (show_depth) {
                my_renderer.setDepthbuffer(depthbuffer_address);
                if (my_renderer.getDepthbuffer().saveToPPMImage(file_name)) {
                    printf("Depthbuffer image saved to: %s.ppm\n", file_name.c_str());
                }
                else {
                    printf("Failed to save depthbuffer image!\n");
                }
            }
            else {
                my_renderer.setFramebuffer(framebuffer_address);
                if (my_renderer.getFramebuffer().saveToPPMImage(file_name)) {
                    printf("Framebuffer image saved to: %s.ppm\n", file_name.c_str());
                }
                else {
                    printf("Failed to save framebuffer image!\n");
                }
            }

            // Reset frame/depthbuffer resolution.
            my_renderer.setResolution(window_width, window_height);
        }

        // Unlock SDL texture.
        SDL_UnlockTexture(sdl_frame_texture);

        // Render SDL texture to screen.
        handleSDLError(SDL_RenderCopy(sdl_renderer, sdl_frame_texture, nullptr, nullptr));

        // Update screen.
        SDL_RenderPresent(sdl_renderer);
    }
}
