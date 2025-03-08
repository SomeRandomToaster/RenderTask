#pragma once

#include "LiteMath/LiteMath.h"

using LiteMath::float3;

class Ray
{
    float3 m_origin, m_direction;
public:
    Ray(const float3& origin, const float3& direction)
        : m_origin(origin), m_direction(normalize(direction)) {}

    float3 origin() const { return m_origin; }
    float3 direction() const { return m_direction; }

    float3 at(float t) const { return m_origin + m_direction * t; }
};
