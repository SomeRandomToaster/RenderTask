#include "RayTracer.h"

#include <thread>

using LiteMath::lerp;
using LiteMath::min;
using LiteMath::max;

float4 RayTracer::get_ray_color(const Ray& ray, size_t depth) const
{
    if (depth >= MAX_DEPTH) {
        return { u_background_color.x, u_background_color.y, u_background_color.z, 1 };
    }
    float3 color = u_background_color;

    Hit hit;
    hit.t = Hittable::MAX_DIST;
    hit.is_mirror = false;

    if (m_hit_list.hit(ray, hit)) {
        if (hit.is_mirror)
            return get_ray_color({ray.at(hit.t), LiteMath::reflect(ray.direction(), hit.normal)}, depth + 1 );
        color = shade_lambert(ray, hit);
    }



    return {color.x, color.y, color.z, 1.0};
}


void RayTracer::draw_frame()
{
    //your rendering code goes here


    for (int k = 0; k < THR_COUNT; k++) {
        threads[k] = std::thread([this, k]() {
            for(int i = 0; i < m_width; i++) {
                for(int j = k * m_height / THR_COUNT; j < (k + 1) * m_height / THR_COUNT; j++) {
                    float3 pixel_center = m_pixel00 + i * m_camera_dx + j * m_camera_dy;

                    Ray ray = {m_camera_pos, pixel_center - m_camera_pos};

                    float4 color = get_ray_color(ray, 0);

                    unsigned pixel_idx = j * m_width + i;

                    uint8_t r = uint8_t(color.x * 255.99);
                    uint8_t g = uint8_t(color.y * 255.99);
                    uint8_t b = uint8_t(color.z * 255.99);
                    uint8_t a = uint8_t(color.w * 255.99);
                    uint32_t color_uint = a << 24 | r << 16 | g << 8 | b;
                    m_pixels[pixel_idx] = color_uint;
                }
            }
        });
    }

    for (int i = 0; i < THR_COUNT; i++) {
        threads[i].join();
    }

}


float3 RayTracer::shade_lambert(const Ray& ray, const Hit& hit) const
{
    constexpr float ambient_factor = 0.1f;
    constexpr float EPS = 1e-1;

    float3 pos = ray.at(hit.t) + hit.normal * EPS;
    float3 to_light = u_light_pos - pos;

    Ray shadow_ray{ ray.at(hit.t - EPS), to_light };
    Hit tmp;
    tmp.t = Hittable::MAX_DIST;

    if (m_hit_list.hit(shadow_ray, tmp)) {
        return hit.color * ambient_factor;
    }


    float diffuse_factor = max(dot(normalize(to_light), hit.normal), 0.0f) * min(u_light_intensity / dot(to_light, to_light), 1 - ambient_factor);
    return hit.color * u_light_color * diffuse_factor + hit.color * ambient_factor;

}


RayTracer::RayTracer(std::vector<uint32_t>& pixels, const HittableList& hit_list, int width, int height, float focal_len)
        : m_focal_len(focal_len), m_width(width), m_height(height), m_pixels(pixels), m_aspect_ratio(float(width) / height), m_hit_list(hit_list)
{

    m_viewport_width = m_viewport_height * m_aspect_ratio;
    update_camera_coordinates();

    m_hor_iter.resize(m_width);
    for (int i = 0; i < m_width; i++) {
        m_hor_iter[i] = i;
    }

    m_vert_iter.resize(m_width);
    for (int i = 0; i < m_width; i++) {
        m_vert_iter[i] = i;
    }
}


void RayTracer::update_camera_coordinates()
{
    using LiteMath::normalize;
    using LiteMath::dot;
    using LiteMath::cross;

    m_camera_z = normalize(m_camera_pivot - m_camera_pos);
    float3 z_proj = normalize({m_camera_z.x, 0, m_camera_z.z});
    float theta = -asin(m_camera_z.y);
    m_camera_y = -(z_proj * sinf(theta) + float3{0.0f, cosf(theta), 0.0f}) * m_viewport_height / 2;
    m_camera_x = cross(m_camera_z, m_camera_y) * m_viewport_width / 2;

    m_camera_dx = 2 * m_camera_x / m_width;
    m_camera_dy = 2 * m_camera_y / m_height;
    m_viewport00 = m_camera_z * m_focal_len - m_camera_x - m_camera_y + m_camera_pos;
    m_pixel00 = m_viewport00 + 0.5 * (m_camera_dx + m_camera_dy);
}


void RayTracer::set_camera_pos(const float3& pos)
{
    m_camera_pos = pos;
    update_camera_coordinates();
}


void RayTracer::set_camera_pivot(const float3& pivot)
{
    m_camera_pivot = pivot;
    update_camera_coordinates();
}

float3 RayTracer::random_direction() {
    using LiteMath::rnd;
    return normalize({ rnd(-1, 1), rnd(-1, 1), rnd(-1, 1) });
}

float3 RayTracer::random_on_hemisphere(const float3& axis) {
    float3 rand_dir = random_direction();
    return dot(axis, rand_dir) < 0 ? -rand_dir : rand_dir;
}