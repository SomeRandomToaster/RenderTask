#include "BVHTree.h"

#include <algorithm>

BVHTree::BVHTree(const std::vector<Triangle>& tris)
    : m_box(tris), m_count(tris.size())
{
    if (tris.size() <= MAX_TRIANGLES_IN_NODE) {
        m_tris = tris;
        return;
    }

    std::vector<Triangle> sorted_x = tris;
    std::sort(sorted_x.begin(), sorted_x.end(), Triangle::compare_x);
    std::vector<Triangle> sorted_y = tris;
    std::sort(sorted_y.begin(), sorted_y.end(), Triangle::compare_y);
    std::vector<Triangle> sorted_z = tris;
    std::sort(sorted_z.begin(), sorted_z.end(), Triangle::compare_z);

    auto subdivision_x = subdivide(sorted_x);
    auto subdivision_y = subdivide(sorted_y);
    auto subdivision_z = subdivide(sorted_z);

    auto min_sah_subdivision = std::min(subdivision_x, std::min(subdivision_y, subdivision_z));

    std::vector<Triangle> left_tris, right_tris;

    left_tris.reserve(min_sah_subdivision.second);
    right_tris.reserve(tris.size() - min_sah_subdivision.second);

    for (int i = 0; i < min_sah_subdivision.second; i++) {
        left_tris.push_back(tris[i]);
    }
    for (int i = min_sah_subdivision.second; i < tris.size(); i++) {
        right_tris.push_back(tris[i]);
    }

    m_left = new BVHTree(left_tris);
    m_right = new BVHTree(right_tris);
}


std::pair<double, int> BVHTree::subdivide(const std::vector<Triangle>& tris)
{
    std::vector<Box> boxes_left(tris.size(), Box({tris[0]}));
    std::vector<Box> boxes_right(tris.size(), Box({tris[tris.size() - 1]}));

    for (int i = 1; i < tris.size(); i++) {
        boxes_left[i] = boxes_left[i - 1];
        boxes_left[i].add(tris[i]);
    }

    for (int i = tris.size() - 2; i >= 0; i--) {
        boxes_right[i] = boxes_right[i + 1];
        boxes_right[i].add(tris[i]);
    }

    float l_surf_area = tris[0].area();

    float r_surf_area = 0.f;
    for (int i = 1; i < tris.size(); i++) {
        r_surf_area += tris[i].area();
    }

    std::pair<double, int> ans { MAXFLOAT, 0 };

    for (int i = 1; i < tris.size(); i++) {
        float l_box_area = boxes_left[i - 1].surface_area();
        float r_box_area = boxes_right[i].surface_area();

        float l_prob = l_surf_area / l_box_area;
        float r_prob = r_surf_area / r_box_area;
        float sah = l_prob * i + r_prob * (tris.size() - i);

        if (sah < ans.first) {
            ans.first = sah;
            ans.second = i;
        }

        l_surf_area += tris[i].area();
        r_surf_area -= tris[i].area();
    }
    return ans;
}

bool BVHTree::hit(const Ray& ray, Hit& hit) const
{
    bool ans = false;
    Hit box_hit = hit;
    bool hit_flag = m_box.hit(ray, box_hit);

    if (!hit_flag) {
        return false;
    }

    if (!m_tris.empty()) {
        for (const Triangle& t : m_tris) {
            ans = t.hit(ray, hit) || ans;
        }
        return ans;
    }

    Hit left_hit, right_hit;
    left_hit.t = right_hit.t = Hittable::MAX_DIST;

    ans = m_left->hit(ray, hit) || ans;
    ans = m_right->hit(ray, hit) || ans;

    return ans;
}
