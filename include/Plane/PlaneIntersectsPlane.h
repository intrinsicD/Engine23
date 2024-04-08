//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANEINTERSECTSPLANE_H
#define ENGINE23_PLANEINTERSECTSPLANE_H

#include "Plane.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Plane<T, N> &plane1, const Plane<T, N> &plane2) {
        return !plane1.normal.isApprox(plane2.normal) || plane1.d == plane2.d;
    }
}

#endif //ENGINE23_PLANEINTERSECTSPLANE_H
