//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYINTERSECTSTRIANGLE_H
#define ENGINE23_RAYINTERSECTSTRIANGLE_H

#include "Ray.h"
#include "Triangle.h"

namespace Bcg {
    template<typename T, int N>
    T IntersectionPoint(const Ray<T, N> &ray, const Triangle<T, N> &triangle) {
        Eigen::Vector<T, N> edge1 = triangle.points.row(1) - triangle.points.row(0);
        Eigen::Vector<T, N> edge2 = triangle.points.row(2) - triangle.points.row(0);
        Eigen::Vector<T, N> h = ray.direction.cross(edge2);
        T a = edge1.dot(h);
        if (a > -std::numeric_limits<T>::epsilon() && a < std::numeric_limits<T>::epsilon()) {
            return -std::numeric_limits<T>::infinity();
        }
        T f = 1 / a;
        Eigen::Vector<T, N> s = ray.origin - triangle.a;
        T u = f * s.dot(h);
        if (u < 0 || u > 1) {
            return -std::numeric_limits<T>::infinity();
        }
        Eigen::Vector<T, N> q = s.cross(edge1);
        T v = f * ray.direction.dot(q);
        if (v < 0 || u + v > 1) {
            return -std::numeric_limits<T>::infinity();
        }
        T t = f * edge2.dot(q);
        if (t > std::numeric_limits<T>::epsilon()) {
            return t;
        }
        return -std::numeric_limits<T>::infinity();

    }

#endif //ENGINE23_RAYINTERSECTSTRIANGLE_H
