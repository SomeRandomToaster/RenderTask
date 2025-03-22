#include "LiteMath/LiteMath.h"
#include "LiteMath/Image2d.h"

#define LOG_TIME

// stb_image is a single-header C library, which means one of your cpp files must have
//    #define STB_IMAGE_IMPLEMENTATION
//    #define STB_IMAGE_WRITE_IMPLEMENTATION
// since Image2d already defines the implementation, we don't need to do that here.
#include "stb_image.h"
#include "stb_image_write.h"

#include <SDL_keycode.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <chrono>
#include <SDL.h>

#include "BVHTree.h"
#include "mesh.h"

#include "RayTracer.h"

using namespace cmesh4;

using LiteMath::float2;
using LiteMath::float3;
using LiteMath::float4;
using LiteMath::int2;
using LiteMath::int3;
using LiteMath::int4;
using LiteMath::uint2;
using LiteMath::uint3;
using LiteMath::uint4;


int main(int argc, char **args)
{
    const int SCREEN_WIDTH = 512;
    const int SCREEN_HEIGHT = 512;

    constexpr float camera_rot_speed = 0.01;
    constexpr float camera_move_speed = 0.05;
    constexpr float light_move_speed = 0.1;
    constexpr float light_trajectory_radius = 0;
    constexpr float light_height = 2;
    constexpr float light_intensity = 3;
    const float3 light_color = {1, 0.85, 0.7};
    const float3 mesh_color = {1, 0.8, 0.9};

    // Pixel buffer (RGBA format)
    std::vector<uint32_t> pixels(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFFFFFFFF); // Initialize with white pixels


    auto plane = Plane(0, 1, 0, -1, {1, 1, 1});
    auto sphere = Sphere({1.5, 0, 0}, 1, {1, 0, 0}, true);

    SimpleMesh mesh = LoadMeshFromObj("res/meshes/spot.obj");

    HittableList hit_list( { &plane, &sphere });

    std::vector<Triangle> mesh_tris;
    mesh_tris.reserve(mesh.TrianglesNum());
    for (int i = 0; i < mesh.IndicesNum(); i += SimpleMesh::POINTS_IN_TRIANGLE) {

        float3 a = to_float3(mesh.vPos4f[mesh.indices[i]]);
        float3 b = to_float3(mesh.vPos4f[mesh.indices[i + 1]]);
        float3 c = to_float3(mesh.vPos4f[mesh.indices[i + 2]]);

        float3 a_normal = to_float3(mesh.vNorm4f[mesh.indices[i]]);
        float3 b_normal = to_float3(mesh.vNorm4f[mesh.indices[i + 1]]);
        float3 c_normal = to_float3(mesh.vNorm4f[mesh.indices[i + 2]]);

        mesh_tris.push_back({a, b, c, a_normal, b_normal, c_normal, mesh_color});
    }
    BVHTree bvh_tree(mesh_tris);
    std::cout << "Triangles count: " << bvh_tree.count() << std::endl;
    hit_list.add(&bvh_tree);

    RayTracer ray_tracer{pixels, hit_list, SCREEN_WIDTH, SCREEN_HEIGHT};

    ray_tracer.u_light_intensity = light_intensity;
    ray_tracer.u_light_color = light_color;

    // Initialize SDL. SDL_Init will return -1 if it fails.
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
      std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
      return 1;
    }

    // Create our window
    SDL_Window *window = SDL_CreateWindow("SDF Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Make sure creating the window succeeded
    if (!window)
    {
      std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
      return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
      std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    // Create a texture
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,    // 32-bit RGBA format
        SDL_TEXTUREACCESS_STREAMING, // Allows us to update the texture
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (!texture)
    {
      std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }

    SDL_Event ev;
    bool running = true;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    unsigned frame_count = 0;

    unsigned long long last_update_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    unsigned long long start_time = last_update_time;

    float r = 3.0f;
    float phi = 0;
    float theta = 0;

    ray_tracer.set_camera_pos({r * sinf(phi) * cosf(theta), r * sinf(theta), -r * cosf(phi) * cosf(theta)});

    // Main loop
    while (running)
    {
        // Event loop
        while (SDL_PollEvent(&ev) != 0)
        {
            // check event type
            switch (ev.type)
            {
            case SDL_QUIT:
                // shut down
                running = false;
                break;
            case SDL_KEYDOWN:
                // test keycode
                switch (ev.key.keysym.sym)
                {
                    case SDLK_UP:
                        theta += camera_rot_speed;
                        break;
                    case SDLK_DOWN:
                        theta -= camera_rot_speed;
                        break;
                    case SDLK_LEFT:
                        phi -= camera_rot_speed;
                        break;
                    case SDLK_RIGHT:
                        phi += camera_rot_speed;
                        break;
                    case SDLK_w:
                        r -= camera_move_speed;
                        break;
                    case SDLK_s:
                        r += camera_move_speed;
                        break;
                    //ESC to exit
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                        // etc
                }
                ray_tracer.set_camera_pos({r * sinf(phi) * cosf(theta), r * sinf(theta), -r * cosf(phi) * cosf(theta)});
                break;

            }
        }

        now = std::chrono::system_clock::now();
        unsigned long long current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        float t = (current_time - start_time) * 1e-9f;
        ray_tracer.u_light_pos = {light_trajectory_radius * cos(light_move_speed * t * LiteMath::M_PI), light_height, light_trajectory_radius * sin(light_move_speed * t * LiteMath::M_PI)};
        ray_tracer.draw_frame();



        // Update the texture with the pixel buffer
        SDL_UpdateTexture(texture, nullptr, pixels.data(), SCREEN_WIDTH * sizeof(uint32_t));

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        // Update the screen
        SDL_RenderPresent(renderer);


        if(current_time - last_update_time >= 1e9) {
            last_update_time = current_time;
            std::cout << frame_count << std::endl;
            frame_count = 0;
        }
        frame_count++;
    }

    // Destroy the window. This will also destroy the surface
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();

    // End the program
    return 0;
}