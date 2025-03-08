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



            float3 pixel_center = m_pixel00 + i * m_viewport_du + j * m_viewport_dv;
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