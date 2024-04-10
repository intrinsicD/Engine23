//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANEINTERSECTSRAY_H
#define ENGINE23_PLANEINTERSECTSRAY_H

#include "Plane.h"
#include "Ray.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Plane<T, N> &plane, const Ray<T, N> &ray) {
        return plane.normal.dot(ray.direction) != 0;
    }
}

#endif //ENGINE23_PLANEINTERSECTSRAY_H
