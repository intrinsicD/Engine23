//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANEINTERSECTSSPHERE_H
#define ENGINE23_PLANEINTERSECTSSPHERE_H

#include "Plane.h"
#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Plane<T, N> &plane, const Sphere<T, N> &sphere) {
        return std::abs(plane.normal.dot(sphere.center) + plane.d) <= sphere.radius;
    }
}
#endif //ENGINE23_PLANEINTERSECTSSPHERE_H
