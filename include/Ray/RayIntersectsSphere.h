//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYINTERSECTSSPHERE_H
#define ENGINE23_RAYINTERSECTSSPHERE_H

#include "Ray.h"
#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    T IntersectionPoint(const Ray<T, N> &ray, const Sphere<T, N> &sphere) {
        Eigen::Vector<T, N> oc = ray.origin - sphere.center;
        T a = ray.direction.squaredNorm();
        T b = 2 * oc.dot(ray.direction);
        T c = oc.squaredNorm() - sphere.radius * sphere.radius;
        T discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return -std::numeric_limits<T>::infinity();
        }
        T t1 = (-b + std::sqrt(discriminant)) / (2 * a);
        T t2 = (-b - std::sqrt(discriminant)) / (2 * a);
        return std::min(t1, t2);
    }
}

#endif //ENGINE23_RAYINTERSECTSSPHERE_H
