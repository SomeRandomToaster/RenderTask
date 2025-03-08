#pragma once

#include <vector>

#include "Ray.h"
#include "Hittable.h"

using LiteMath::float4;

class RayTracer
{
private:
    float m_focal_len, m_aspect_ratio;
    unsigned m_width, m_height;
    float m_viewport_height = 2.0;
    float m_viewport_width;
    static constexpr float MAX_DIST = 100.f;

    std::vector<uint32_t>& m_pixels;
    HittableList m_hit_list;

    float3 m_viewport_u;
    float3 m_viewport_v{ 0, -m_viewport_height, 0 };
    float3 m_viewport_du, m_viewport_dv;
    float3 m_viewport00;
    float3 m_camera_pos{ 0, 0, 0 };
    float3 m_pixel00;

public:
    float3 u_background_color{ 0.0, 0.0, 0.0 };
    float3 u_light_pos{ 5, 10, -10 };
    float3 u_light_color{ 1, 1, 1 };
    float u_light_intensity = 1000.0f;
public:
    void draw_frame();
    RayTracer(std::vector<uint32_t>& pixels, const HittableList& hit_list, int width, int height, float focal_len = 1.0)
        : m_focal_len(focal_len), m_width(width), m_height(height), m_pixels(pixels), m_aspect_ratio(float(width) / height), m_hit_list(hit_list)
    {
        m_viewport_width = m_viewport_height * m_aspect_ratio;
        m_viewport_u = {m_viewport_width, 0, 0};
        m_viewport_du = m_viewport_u / m_width;
        m_viewport_dv = m_viewport_v / m_height;
        m_viewport00 = float3{ 0, 0, focal_len } - m_viewport_u/2 - m_viewport_v / 2 - m_camera_pos;
        m_pixel00 = m_viewport00 + 0.5 * (m_viewport_du + m_viewport_dv);
    }
private:
    float4 get_ray_color(const Ray& ray) const;
    float3 shade_lambert(const Ray& ray, const Hit& hit) const;
    static uint32_t convert_color(const float4& color);
};
