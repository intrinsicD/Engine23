//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHEREINTERSECTSAABB_H
#define ENGINE23_SPHEREINTERSECTSAABB_H

#include "Sphere.h"
#include "AABBClosestPoint.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Sphere<T, N> &sphere, const AABB<T, N> &aabb) {
        return (ClosestPoint(sphere.center, aabb) - sphere.center).squaredNorm() <= sphere.radius * sphere.radius;
    }

}

#endif //ENGINE23_SPHEREINTERSECTSAABB_H
