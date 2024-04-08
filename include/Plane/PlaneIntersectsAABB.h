//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANEINTERSECTSAABB_H
#define ENGINE23_PLANEINTERSECTSAABB_H

#include "Plane.h"
#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const Plane<T, N> &plane, const AABB<T, N> &aabb) {
        // Initialize distances
        T distMin = 0, distMax = 0;

        // Compute distances from the AABB extremes to the plane
        for (int i = 0; i < N; ++i) {
            if (plane.normal[i] >= 0) {
                distMin += plane.normal[i] * aabb.min[i];
                distMax += plane.normal[i] * aabb.max[i];
            } else {
                distMin += plane.normal[i] * aabb.max[i];
                distMax += plane.normal[i] * aabb.min[i];
            }
        }

        // Check for intersection
        if ((distMin + plane.d) > 0 || (distMax + plane.d) < 0) {
            return false;  // No intersection
        }

        // Considering an epsilon for numerical stability might be necessary here
        return true;  // Intersects
    }
}

#endif //ENGINE23_PLANEINTERSECTSAABB_H
