//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHEREINTERSECTSSPHERE_H
#define ENGINE23_SPHEREINTERSECTSSPHERE_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Sphere<T, N> &sphere, const Sphere<T, N> &other) {
        return (sphere.center - other.center).squaredNorm() <=
               (sphere.radius + other.radius) * (sphere.radius + other.radius);
    }
}

#endif //ENGINE23_SPHEREINTERSECTSSPHERE_H
