#pragma once

#include "Triangle.h"

class Box : public Hittable
{
    float3 m_min;
    float3 m_max;
public:
    Box(const float3& min, const float3& max)
        : m_min(min), m_max(max) {}

    Box(const std::vector<Triangle>& v);
    virtual bool hit(const Ray& ray, Hit& hit) const override;
    float3 min() const { return m_min; }
    float3 max() const { return m_max; }
    void add(const Triangle& t);
    float surface_area() const;
};
