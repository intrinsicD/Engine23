//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERECONTAINSAABB_H
#define ENGINE23_SPHERECONTAINSAABB_H

#include "Sphere.h"
#include "AABB.h"

namespace Bcg {

    template<typename T, int N>
    bool Contains(const Sphere<T, N> &sphere, const AABB<T, N> &aabb) {
        T distance = 0;
        for (size_t i = 0; i < N; ++i) {
            if (sphere.center[i] < aabb.min[i]) {
                distance += (sphere.center[i] - aabb.min[i]) * (sphere.center[i] - aabb.min[i]);
            } else if (sphere.center[i] > aabb.max[i]) {
                distance += (sphere.center[i] - aabb.max[i]) * (sphere.center[i] - aabb.max[i]);
            }
        }
        return distance <= sphere.radius * sphere.radius;
    }

    template<typename T, int N>
    bool Contains(const AABB<T, N> &aabb, const Sphere<T, N> &sphere) {
        return Contains(sphere, aabb);
    }

}

#endif //ENGINE23_SPHERECONTAINSAABB_H
