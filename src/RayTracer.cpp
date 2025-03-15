#include "RayTracer.h"

using LiteMath::lerp;
using LiteMath::min;
using LiteMath::max;

float4 RayTracer::get_ray_color(const Ray& ray) const
{
    float3 color = u_background_color;

    static struct Hit hit;
    hit.t = MAX_DIST;


    if(m_hit_list.hit(ray, hit)) {
        color = shade_lambert(ray, hit);
    }


    return {color.x, color.y, color.z, 1.0};
}


void RayTracer::draw_frame()
{
    //your rendering code goes here

    for(int i = 0; i < m_width; i++) {
        for(int j = 0; j < m_height; j++) {



            float3 pixel_center = m_pixel00 + i * m_camera_dx + j * m_camera_dy;
            Ray ray = {m_camera_pos, pixel_center - m_camera_pos};



            float4 color = get_ray_color(ray);

            unsigned pixel_idx = j * m_width + i;
            m_pixels[pixel_idx] = convert_color(color);

        }
    }

}


uint32_t RayTracer::convert_color(const float4& color) {
    uint8_t r = uint8_t(color.x * 255);
    uint8_t g = uint8_t(color.y * 255);
    uint8_t b = uint8_t(color.z * 255);
    uint8_t a = uint8_t(color.w * 255);
    return a << 24 | r << 16 | g << 8 | b;
}


float3 RayTracer::shade_lambert(const Ray& ray, const Hit& hit) const
{
    float3 pos = ray.at(hit.t);
    float3 to_light = u_light_pos - pos;

    float ambient_factor = 0.2f;
    float diffuse_factor = max(dot(normalize(to_light), hit.normal), 0.0f) * min(u_light_intensity / dot(to_light, to_light), 1 - ambient_factor);
    return hit.color * u_light_color * diffuse_factor + hit.color * ambient_factor;

}


RayTracer::RayTracer(std::vector<uint32_t>& pixels, const HittableList& hit_list, int width, int height, float focal_len)
        : m_focal_len(focal_len), m_width(width), m_height(height), m_pixels(pixels), m_aspect_ratio(float(width) / height), m_hit_list(hit_list)
{

    m_viewport_width = m_viewport_height * m_aspect_ratio;
    update_camera_coordinates();
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