//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYINTERSECTSSPHERE_H
#define ENGINE23_RAYINTERSECTSSPHERE_H

#include "Ray.h"
#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Ray<T, N> &ray, const Sphere<T, N> &sphere) {
        Eigen::Vector<T, N> oc = ray.origin - sphere.center;
        T a = ray.direction.squaredNorm();
        T b = 2 * oc.dot(ray.direction);
        T c = oc.squaredNorm() - sphere.radius * sphere.radius;
        T discriminant = b * b - 4 * a * c;
        return discriminant >= 0;
    }
}

#endif //ENGINE23_RAYINTERSECTSSPHERE_H
