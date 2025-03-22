#pragma once

#include "LiteMath/LiteMath.h"
#include "Ray.h"
#include <vector>

using LiteMath::length;
using LiteMath::normalize;
using LiteMath::float3;

struct Hit
{
    float t;
    bool is_mirror;
    float3 normal;
    float3 color;
};

class Hittable
{
public:
    virtual bool hit(const Ray& ray, Hit& hit) const = 0;
    static constexpr float MAX_DIST = 100.f;
};


class Plane : public Hittable
{
    float3 m_normal;
    float m_offset;
    float3 m_color;
    bool m_is_mirror;
public:
    Plane(float a, float b, float c, float d, const float3& color, bool is_mirror = false)
        : m_normal(normalize({a, b, c})), m_offset(d / length({a, b, c})), m_color(color), m_is_mirror(is_mirror) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};


class Sphere : public Hittable
{
    float3 m_center;
    float m_radius;
    float3 m_color;
    bool m_is_mirror;
public:
    Sphere(const float3& center, float radius, const float3& color, bool is_mirror = false)
        : m_center(center), m_radius(radius), m_color(color), m_is_mirror(is_mirror) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};


class HittableList : public Hittable
{
    std::vector<const Hittable*> list;
public:
    HittableList(const std::vector<const Hittable*>& vector)
        : list(vector) {}
    void add(const Hittable* item) { list.push_back(item); }
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};

