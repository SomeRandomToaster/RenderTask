#include "Triangle.h"

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
        hit.is_mirror = m_is_mirror;
        hit.normal = (tuv_coords.y * m_normal_a + tuv_coords.z * m_normal_a + t1 * m_normal_c);
        return true;
    }
    return false;
}


bool Triangle::compare_x(const Triangle& left, const Triangle& right)
{
    using LiteMath::max;
    float left_max_x = max(left.m_a.x, max(left.m_b.x, left.m_c.x));
    float right_max_x = max(right.m_a.x, max(right.m_b.x, right.m_c.x));
    return left_max_x < right_max_x;
}


bool Triangle::compare_y(const Triangle& left, const Triangle& right)
{
    using LiteMath::max;
    float left_max_y = max(left.m_a.y, max(left.m_b.y, left.m_c.y));
    float right_max_y = max(right.m_a.y, max(right.m_b.y, right.m_c.y));
    return left_max_y < right_max_y;
}


bool Triangle::compare_z(const Triangle& left, const Triangle& right)
{
    using LiteMath::max;
    float left_max_z = max(left.m_a.z, max(left.m_b.z, left.m_c.z));
    float right_max_z = max(right.m_a.z, max(right.m_b.z, right.m_c.z));
    return left_max_z < right_max_z;
}