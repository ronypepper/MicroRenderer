//
// Created by pusdo on 13/08/2024.
//

#include "SDL.h"
//#include "fpm/fixed.hpp"
#include "MicroRenderer/MicroRenderer.h"

using namespace MicroRenderer;

#define SHADER_SIMPLECOUNTOURS 0
#define SHADER_GOURAUDTEXTURED 1

#define MODE_FREEFLY 0
#define MODE_CAPTURE_STATS 1

// ------------------ Renderer configuration --------------------- //

using DataType = double;//fpm::fixed_16_16;
#define DEMO_MODE MODE_FREEFLY
#define USED_SHADER SHADER_GOURAUDTEXTURED
constexpr int32 window_width = 1200;
constexpr int32 window_height = 1200;
constexpr ShaderConfiguration my_shader_cfg = {
	PERSPECTIVE, CULL_AT_SCREEN_BORDER, CLIP_AT_NEAR_PLANE, DEPTH_TEST_ENABLED, SHADING_ENABLED,
	{FORMAT_RGB888, TYPE_NORMALIZED}
};
constexpr RendererConfiguration my_renderer_cfg = {SCANLINE, CLOCKWISE, my_shader_cfg};

// ------------------ Renderer configuration --------------------- //

// ------------------ Model and Texture includes --------------------- //

#if USED_SHADER == SHADER_SIMPLECOUNTOURS
#include "models/SimpleContours/cube.h"
#include "models/SimpleContours/tu_vienna_logo.h"
#include "models/SimpleContours/stanford_bunny.h"
#elif USED_SHADER == SHADER_GOURAUDTEXTURED
#include "models/GouraudTextured/cube.h"
#include "models/GouraudTextured/plane.h"
#include "textures/test_texture.h"
#include "textures/tu_logo_texture.h"
#endif

// ------------------ Model and Texture includes --------------------- //

// ------------------ Renderer globals --------------------- //

// Renderer object.
#if USED_SHADER == SHADER_SIMPLECOUNTOURS
using MyRenderer = Renderer<DataType, my_renderer_cfg, SimpleContoursShaderProgram>;
#elif USED_SHADER == SHADER_GOURAUDTEXTURED
using MyRenderer = Renderer<DataType, my_renderer_cfg, GouraudTexturedShaderProgram>;
#endif
using MyShaderProgram = MyRenderer::ShaderProgram_type;
MyRenderer my_renderer;

// Buffers.
MyRenderer::VertexBuffer vertex_buffer[1000];
MyRenderer::RasterizationBuffer rasterization_buffers[400];
MyRenderer::RasterizationOrder rasterization_order[400];

// Frame/depthbuffer.
void* framebuffer_address = nullptr;
DataType depthbuffer_address[window_width * window_height];

// Screen-projection transform.
Matrix4<DataType> screen_proj_tf;

// View state.
Vector3<DataType> view_position = {0., 0., 0.};
Vector3<DataType> view_orientation = {0., 0., 0.};
constexpr DataType view_translation_speed = 2.;
constexpr DataType view_rotation_speed = 30.;
Vector3<DataType> view_fwd_dir, view_up_dir, view_right_dir;

// ------------------ Renderer globals --------------------- //

// ------------------ Shader globals --------------------- //

#if USED_SHADER == USE_SIMPLECOUNTOURS_SHADER
// Triangle normals.
Vector3<DataType> cube_tri_normals[cube_triangle_number];
Vector3<DataType> tu_vienna_logo_tri_normals[tu_vienna_logo_triangle_number];
Vector3<DataType> stanford_bunny_tri_normals[stanford_bunny_triangle_number];

// Lighting.
Vector3<DataType> towards_sun_dir_world_space = Vector3<DataType>(2.0, 0.3, 0.7).getNormalized();
PointLight<DataType> world_point_lights[3] = {
	{{2.3, -0.1, 2.8}, {3.0, 0.1, 0.1}},
	{{-1.5, -1.3, 2.7}, {0.1, 3.0, 0.1}},
	{{0.0, -0.3, 2.5}, {0.1, 0.1, 3.0}}
};

// Global data.
MyRenderer::GlobalData global_data = {};

// Instance data.
constexpr uint16 num_instances = 3;
MyRenderer::InstanceData instances[num_instances] = {
	{2, {}, tu_vienna_logo_tri_normals, {}, {0.0, 0.0, 1.0}},
	{0, {}, cube_tri_normals, {}, {1.0, 0.0, 0.0}},
	{0, {}, cube_tri_normals, {}, {0.0, 1.0, 0.0}}
};

// Models.
constexpr MyRenderer::ModelData models[] = {
	cube_model<DataType, my_shader_cfg>,
	cube_model<DataType, my_shader_cfg>,
	tu_vienna_logo_model<DataType, my_shader_cfg>
};
#elif USED_SHADER == SHADER_GOURAUDTEXTURED
// Global data.
MyRenderer::GlobalData global_data = {
    0.3
};

// Instance data.
constexpr uint16 num_instances = 3;
MyRenderer::InstanceData instances[num_instances] = {
    {0 ,{1.0}, {test_texture_rgb888, test_texture_width, test_texture_height} ,{}},
    {1 ,{1.0}, {tu_logo_texture, tu_logo_texture_width, tu_logo_texture_height}, {}},
    {1 ,{1.0}, {tu_logo_texture, tu_logo_texture_width, tu_logo_texture_height}, {}}
};

// Models.
constexpr MyRenderer::ModelData models[] = {
	cube_model<DataType, my_shader_cfg>,
	plane_model<DataType, my_shader_cfg>,
	plane_model<DataType, my_shader_cfg>
};
#endif

// Cube state.
DataType objects_y_rot = 180.0;
DataType cubes_rot_speed = 15.0;

// ------------------ Shader globals --------------------- //

// ------------------ Rendering functions --------------------- //

void initializeRenderer()
{
	// Set resolution.
	my_renderer.setResolution(window_width, window_height);

	// Compute screen-projection transform.
	screen_proj_tf = Transform::viewport<DataType>(window_width, window_height, false, false);
	if constexpr (my_shader_cfg.projection == PERSPECTIVE) {
		screen_proj_tf *= Transform::perspectiveProjection<DataType>(-0.02, 0.02, -0.02, 0.02, 0.1, 15.);
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
	my_renderer.setRasterizationBuffers(rasterization_buffers, rasterization_order);
}

void updateRenderer(DataType delta_time)
{
	// Compute camera directions and transform.
	Matrix3<DataType> view_rotation = Transform::rotationEuler(view_orientation).getMatrix3();
	view_fwd_dir = view_rotation * Vector3<DataType>(0., 0., 1.);
	view_up_dir = view_rotation * Vector3<DataType>(0., 1., 0.);
	view_right_dir = view_rotation * Vector3<DataType>(1., 0., 0.);
	const Matrix4<DataType> view_tf = Transform::camera<DataType>(view_position, view_fwd_dir, view_up_dir);

	// Compute screen-projection-camera transform.
	const Matrix4<DataType> screen_proj_view_tf = screen_proj_tf * view_tf;

	// Rotate cubes.
	objects_y_rot += cubes_rot_speed * delta_time;

#if USED_SHADER == SHADER_SIMPLECOUNTOURS
	// TU Vienna Logo transform.
    Matrix4<DataType> tu_logo_model_tf = Transform::translation<DataType>(Vector3<DataType>(0., 0., 3.5));
    tu_logo_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., objects_y_rot, 0.));
    tu_logo_model_tf *= Transform::scale<DataType>(Vector3<DataType>(0.3));
    instances[0].model_screen_tf = screen_proj_view_tf * tu_logo_model_tf;
    Vector3<DataType> tu_logo_sun_dir = tu_logo_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[0].towards_sun_dir_model_space = tu_logo_sun_dir.getNormalized();
    // Cube 1 transform.
    Matrix4<DataType> cube_1_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1.1, 0.2, 3.2));
    cube_1_model_tf *= Transform::rotationEuler(Vector3<DataType>(0., objects_y_rot, 0.));
    cube_1_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[1].model_screen_tf = screen_proj_view_tf * cube_1_model_tf;
    Vector3<DataType> cube_1_sun_dir = cube_1_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[1].towards_sun_dir_model_space = cube_1_sun_dir.getNormalized();
    // Cube 2 transform.
    Matrix4<DataType> cube_2_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1.3, 0.4, 3.1));
    cube_2_model_tf *= Transform::rotationEuler(Vector3<DataType>(20., 10. + objects_y_rot, 35.));
    cube_2_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[2].model_screen_tf = screen_proj_view_tf * cube_2_model_tf;
    Vector3<DataType> cube_2_sun_dir = cube_2_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    instances[2].towards_sun_dir_model_space = cube_2_sun_dir.getNormalized();
#elif USED_SHADER == SHADER_GOURAUDTEXTURED
    // Cube transform.
    Matrix4<DataType> cube_model_tf = Transform::translation<DataType>(Vector3<DataType>(-1., 0., 3.5));
    cube_model_tf *= Transform::rotationEuler(Vector3<DataType>(0., objects_y_rot, 0.));
    cube_model_tf *= Transform::scale(Vector3<DataType>(0.5));
    instances[0].model_screen_tf = screen_proj_view_tf * cube_model_tf;
    // Vector3<DataType> cube_1_sun_dir = cube_1_model_tf.getMatrix3().getTranspose() * towards_sun_dir_world_space;
    // for (int32 i = 0; i < 3; ++ i) {
    //     instances[0].point_lights[i] = {
    //         world_point_lights[i].intensity,
    //         world_point_lights[i].position;
    // }
    // Plane 1 transform.
    DataType plane_scale_mod = static_cast<DataType>(tu_logo_texture_width) / static_cast<DataType>(tu_logo_texture_height);
    Matrix4<DataType> plane_1_model_tf = Transform::translation<DataType>(Vector3<DataType>(1., 0., 3.5));
    plane_1_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., 180. + objects_y_rot, 0.));
    plane_1_model_tf *= Transform::scale<DataType>(Vector3<DataType>(plane_scale_mod * 0.5, 0.5, 0.5));
    instances[1].model_screen_tf = screen_proj_view_tf * plane_1_model_tf;
    // Plane 2 transform.
    Matrix4<DataType> plane_2_model_tf = Transform::translation<DataType>(Vector3<DataType>(1., 0., 3.5));
    plane_2_model_tf *= Transform::rotationEuler<DataType>(Vector3<DataType>(0., objects_y_rot, 0.));
    plane_2_model_tf *= Transform::scale<DataType>(Vector3<DataType>(plane_scale_mod * 0.5, 0.5, 0.5));
    instances[2].model_screen_tf = screen_proj_view_tf * plane_2_model_tf;
#endif
}

void clearRenderer()
{
	// Update renderer's framebuffer and depthbuffer address.
	my_renderer.setFramebuffer(framebuffer_address);
	my_renderer.setDepthbuffer(depthbuffer_address);

	// Clear renderer's framebuffer and depthbuffer.
	my_renderer.getFramebuffer().setResolution(window_width, window_height);
	my_renderer.getFramebuffer().clearBuffer({0.f});
	my_renderer.getDepthbuffer().setResolution(window_width, window_height);
	my_renderer.getDepthbuffer().clearBuffer(0.f);
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

// ------------------ Rendering helper functions --------------------- //

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
uint32 last_frame_ticks = 0;
DataType delta_time = 0.;
uint32 delta_milliseconds = 0;

// Render time.
constexpr uint32 capture_num_frames = 300;
uint32 min_render_time = 0xFFFFFFFF;
uint32 max_render_time = 0;
uint32 sum_render_time = 0;
uint32 num_frames = 0;

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

	last_frame_ticks = SDL_GetTicks();

	initializeRenderer();

#if DEMO_MODE == MODE_CAPTURE_STATS
	printf("CAPTURING STATS ...");
#endif
}

void update()
{
	// Compute delta time.
    const uint32 current_ticks = SDL_GetTicks();
    delta_milliseconds = current_ticks - last_frame_ticks;
    delta_time = static_cast<DataType>(delta_milliseconds) / static_cast<DataType>(1000.0);
    last_frame_ticks = current_ticks;

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

    updateRenderer(delta_time);
}

void draw() {
#if DEMO_MODE == MODE_FREEFLY
	// Clear call.
	clearRenderer();

	// Take ticks to measure fps.
	const uint32 ticks_stamp = SDL_GetTicks();

	// Draw call.
	drawRenderer();

	// Compute fps.
	const double fps = 1000.0 / static_cast<double>(SDL_GetTicks() - ticks_stamp);

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

	// Print FPS info to console.
	printf((std::string("\nFPS: ") + std::to_string(fps)).c_str());
#elif DEMO_MODE == MODE_CAPTURE_STATS
	if (num_frames < capture_num_frames) {
		// Clear call.
		clearRenderer();

		// Take ticks to measure rendering time.
		const uint32 ticks_stamp = SDL_GetTicks();

		// Draw call.
		drawRenderer();

		// Update rendering time stats.
		const uint32 render_time = SDL_GetTicks() - ticks_stamp;
		sum_render_time += render_time;
		if (render_time < min_render_time)
			min_render_time = render_time;
		if (render_time > max_render_time)
			max_render_time = render_time;

		// Print remaining frames info to console.
		++num_frames;
		printf((std::string("\nFRAMES: ") + std::to_string(num_frames) + std::string("/") + std::to_string(capture_num_frames)).c_str());
	}
	else {
		// Print rendering time stats info to console.
		printf("\n\nSTATS CAPTURED.");
		const double avg_time = static_cast<double>(sum_render_time) / static_cast<double>(capture_num_frames);
		printf((std::string("\nAvg [ms]: ") + std::to_string(avg_time)).c_str());
		const double min_time = static_cast<double>(min_render_time);
		printf((std::string("\nMin [ms]: ") + std::to_string(min_time)).c_str());
		const double max_time = static_cast<double>(max_render_time);
		printf((std::string("\nMax [ms]: ") + std::to_string(max_time)).c_str());
		exit(0);
	}
#endif
}

// ------------------ SDL Demo --------------------- //

int main(int argc, char *argv[])
{
    init();

	// Update and draw loop.
    while (true) {
        // Lock SDL texture.
        int framebuffer_pitch = 0;
        handleSDLError(SDL_LockTexture(sdl_frame_texture, nullptr, &framebuffer_address, &framebuffer_pitch));

    	update();
    	draw();

        //printf("X: %f, Y: %f, Z: %f\n", view_position.x, view_position.y, view_position.z);

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
