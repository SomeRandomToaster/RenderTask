#include "Box.h"


Box::Box(const std::vector<Triangle>& v)
{
    m_min = { MAX_DIST, MAX_DIST, MAX_DIST };
    m_max = -m_min;


    for (const Triangle& t : v) {
        add(t);
    }
}


void Box::add(const Triangle& t)
{
    using LiteMath::min;
    using LiteMath::max;

    m_min.x = min(m_min.x, min(t.a().x, min(t.b().x, t.c().x)));
    m_min.y = min(m_min.y, min(t.a().y, min(t.b().y, t.c().y)));
    m_min.z = min(m_min.z, min(t.a().z, min(t.b().z, t.c().z)));

    m_max.x = max(m_max.x, max(t.a().x, max(t.b().x, t.c().x)));
    m_max.y = max(m_max.y, max(t.a().y, max(t.b().y, t.c().y)));
    m_max.z = max(m_max.z, max(t.a().z, max(t.b().z, t.c().z)));
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


float Box::surface_area() const
{
    return 2.f * ((m_max.x - m_min.x) * (m_max.y - m_min.y) + (m_max.x - m_min.x) * (m_max.z - m_min.z) + (m_max.y - m_min.y) * (m_max.z - m_min.z));
}