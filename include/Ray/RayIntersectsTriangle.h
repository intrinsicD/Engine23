//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYINTERSECTSTRIANGLE_H
#define ENGINE23_RAYINTERSECTSTRIANGLE_H

#include "Ray.h"
#include "Triangle.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Ray<T, N> &ray, const Triangle<T, N> &triangle) {
        Eigen::Vector<T, N> edge1 = triangle.points.row(1) - triangle.points.row(0);
        Eigen::Vector<T, N> edge2 = triangle.points.row(2) - triangle.points.row(0);
        Eigen::Vector<T, N> h = ray.direction.cross(edge2);
        T a = edge1.dot(h);
        if (a > -std::numeric_limits<T>::epsilon() && a < std::numeric_limits<T>::epsilon()) {
            return false;
        }
        T f = 1 / a;
        Eigen::Vector<T, N> s = ray.origin - triangle.a;
        T u = f * s.dot(h);
        if (u < 0 || u > 1) {
            return false;
        }
        Eigen::Vector<T, N> q = s.cross(edge1);
        T v = f * ray.direction.dot(q);
        if (v < 0 || u + v > 1) {
            return false;
        }
        T t = f * edge2.dot(q);
        return t > std::numeric_limits<T>::epsilon();

    }

#endif //ENGINE23_RAYINTERSECTSTRIANGLE_H
