//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPHEREINTERSECTSRAY_H
#define ENGINE23_SPHEREINTERSECTSRAY_H

#include "RayIntersectsSphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Sphere<T, N> &sphere, const Ray<T, N> &ray) {
        return Intersects(ray, sphere);
    }

}

#endif //ENGINE23_SPHEREINTERSECTSRAY_H
