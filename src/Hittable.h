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


class Box : public Hittable
{
    float3 m_min;
    float3 m_max;
public:
    Box(const float3& min, const float3& max)
        : m_min(min), m_max(max) {}
    virtual bool hit(const Ray& ray, Hit& hit) const override;
};

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

