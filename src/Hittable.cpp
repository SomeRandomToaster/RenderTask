#include "Hittable.h"

#include <cmath>

using LiteMath::dot;

bool Plane::hit(const Ray& ray, Hit& hit) const
{
    if(dot(m_normal, ray.direction()) == 0.0) {
        return false;
    }
    float t = (m_offset - dot(m_normal, ray.origin())) / dot(m_normal, ray.direction());

    if(t > 0 && t < hit.t) {
        hit.normal = m_normal;
        hit.color = m_color;
        hit.t = t;
        return true;
    }
    return false;
}


bool Sphere::hit(const Ray &ray, Hit &hit) const
{
    float3 rel_pos = m_center - ray.origin();

    float b_coef = dot(rel_pos, ray.direction());
    float d = b_coef * b_coef - (dot(rel_pos, rel_pos) - m_radius * m_radius);

    if (d < 0) {
        return false;
    }

    float t = b_coef - sqrt(d);

    if (t > 0 && t < hit.t) {
        hit.t = t;
        hit.normal = normalize(ray.at(hit.t) - m_center);
        hit.color = m_color;
        return true;
    }
    return false;
}


bool HittableList::hit(const Ray& ray, Hit& hit) const
{
    bool ans = false;
    for (const Hittable* ptr: list) {
        ans = ptr->hit(ray, hit) || ans;
    }
    return ans;
}


bool Box::hit(const Ray& ray, Hit& hit) const
{
    using LiteMath::min;
    using LiteMath::max;

    float3 inv_dir = 1.0 / ray.direction();

    float lo = inv_dir.x*(m_min.x - ray.origin().x);
    float hi = inv_dir.x*(m_max.x - ray.origin().x);

    float t_min  = min(lo, hi);
    float t_max = max(lo, hi);
    
    float lo1 = inv_dir.y*(m_min.y - ray.origin().y);
    float hi1 = inv_dir.y*(m_max.y - ray.origin().y);

    t_min  = max(t_min, min(lo1, hi1));
    t_max = min(t_max, max(lo1, hi1));

    float lo2 = inv_dir.z*(m_min.z - ray.origin().z);
    float hi2 = inv_dir.z*(m_max.z - ray.origin().z);

    t_min  = max(t_min, min(lo2, hi2));
    t_max = min(t_max, max(lo2, hi2));
    
    if((t_min <= t_max) && (t_max > 0.f) && t_min < hit.t) {
        hit.t = t_min;
        return true;
    }
    return false;
}


bool Triangle::hit(const Ray& ray, Hit& hit) const
{
    using LiteMath::cross;

    float3 t = ray.origin() - m_a;
    float3 p = cross(ray.direction(), m_e2);
    float3 q = cross(t, m_e1);

    float3 tuv_coords = float3(dot(q, m_e2), dot(p, t), dot(q, ray.direction())) / dot(p, m_e1);
    float t1 = 1.0f - tuv_coords.y - tuv_coords.z;
    if (tuv_coords.x < hit.t && tuv_coords.y >= 0 && tuv_coords.z >= 0 && t1 >= 0) {
        hit.t = tuv_coords.x;
        hit.color = m_color;
        hit.normal = tuv_coords.y * m_normal_a + tuv_coords.z * m_normal_a + t1 * m_normal_c;
        return true;
    }
    return false;
}