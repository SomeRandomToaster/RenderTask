#pragma once

#include <vector>
#include <thread>

#include "Ray.h"
#include "Hittable.h"

using LiteMath::float4;

class RayTracer
{
    static constexpr size_t THR_COUNT = 16;
private:
    float m_focal_len, m_aspect_ratio;
    unsigned m_width, m_height;
    float m_viewport_height = 2.0;
    float m_viewport_width;

    std::vector<uint32_t>& m_pixels;
    HittableList m_hit_list;

    float3 m_camera_x;
    float3 m_camera_y;
    float3 m_camera_z;
    float3 m_camera_dx, m_camera_dy;
    float3 m_viewport00;
    float3 m_camera_pos{ 3, 3, -3 };
    float3 m_camera_pivot{ 0, 0, 0 };
    float3 m_pixel00;

    std::vector<int> m_hor_iter, m_vert_iter;

    std::thread threads[THR_COUNT];
public:
    float3 u_background_color{ 0, 0, 0 };
    float3 u_light_pos{ 5, 10, -10 };
    float3 u_light_color{ 1, 1, 1 };
    float u_light_intensity = 5.0f;

    static constexpr size_t MAX_DEPTH  = 5;

public:
    void draw_frame();
    void set_camera_pos(const float3& pos);
    void set_camera_pivot(const float3& pivot);
    RayTracer(std::vector<uint32_t>& pixels, const HittableList& hit_list, int width, int height, float focal_len = 1.0);
private:
    float4 get_ray_color(const Ray& , size_t depth) const;
    float3 shade_lambert(const Ray& ray, const Hit& hit) const;
    void update_camera_coordinates();
    static uint32_t convert_color(const float4& color);
    static float3 random_direction();
    static float3 random_on_hemisphere(const float3& axis);
};
