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
    float3 normal;
    float3 color;
};

class Hittable
{
public:
    virtual bool hit(const Ray& ray, Hit& hit) const = 0;
};


class Plane : public Hittable
{
    float3 m_normal;
    float m_offset;
    float3 m_color;
public:
    Plane(float a, float b, float c, float d, const float3& color)
        : m_normal(normalize({a, b, c})), m_offset(d / length({a, b, c})), m_color(color) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};


class Sphere : public Hittable
{
    float3 m_center;
    float m_radius;
    float3 m_color;
public:
    Sphere(const float3& center, float radius, const float3& color)
        : m_center(center), m_radius(radius), m_color(color) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};


class HittableList : public Hittable
{
    std::vector<const Hittable*> list;
public:
    HittableList(const std::vector<const Hittable*>& vector)
        : list(vector) {}
    void add(const Hittable& item) { list.push_back(&item); }
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};

