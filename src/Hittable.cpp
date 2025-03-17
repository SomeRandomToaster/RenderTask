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


