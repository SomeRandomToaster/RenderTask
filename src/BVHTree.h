#pragma once

#include <memory>

#include "Box.h"

#include <tuple>

class BVHTree : public Hittable
{
    Box m_box;
    std::vector<Triangle> m_tris;
    BVHTree* m_left = nullptr;
    BVHTree* m_right = nullptr;
    size_t m_count;

    std::pair<double, int> subdivide(const std::vector<Triangle>& tris);
public:
    static constexpr unsigned MAX_TRIANGLES_IN_NODE = 2;

    BVHTree(const std::vector<Triangle>& tris);
    ~BVHTree() { delete m_left; delete m_right; }
    virtual bool hit(const Ray& ray, Hit& hit) const override;
    const Box& box() const { return m_box; }
    size_t count() const { return m_count; }

};