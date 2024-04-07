//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERECONTAINSSPHERE_H
#define ENGINE23_SPHERECONTAINSSPHERE_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Contains(const Sphere<T, N> &sphere, const Sphere<T, N> &other) {
        return (sphere.center - other.center).norm() + other.radius <= sphere.radius;
    }
}

#endif //ENGINE23_SPHERECONTAINSSPHERE_H
