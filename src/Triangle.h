#pragma once

#include "Hittable.h"

class Triangle : public Hittable
{
    float3 m_a, m_b, m_c, m_e1, m_e2, m_color;
    float3 m_normal_a, m_normal_b, m_normal_c;
public:
    Triangle(const float3& a, const float3& b, const float3& c,
        const float3& normal_a, const float3& normal_b, const float3& normal_c, const float3& color)
        : m_a(a), m_b(b), m_c(c), m_e1(m_b - m_a), m_e2(m_c - m_a), m_color(color),
          m_normal_a(normal_a), m_normal_b(normal_b), m_normal_c(normal_c) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
    float3 a() const { return m_a; }
    float3 b() const { return m_b; }
    float3 c() const { return m_c; }
    float area() const { return 0.5 * length(LiteMath::cross(m_e1, m_e2)); }

    static bool compare_x(const Triangle& left, const Triangle& right);
    static bool compare_y(const Triangle& left, const Triangle& right);
    static bool compare_z(const Triangle& left, const Triangle& right);
};
