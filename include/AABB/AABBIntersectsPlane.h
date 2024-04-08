//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_AABBINTERSECTSPLANE_H
#define ENGINE23_AABBINTERSECTSPLANE_H

#include "PlaneIntersectsAABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const AABB<T, N> &aabb, const Plane<T, N> &plane) {
        return Intersects(plane, aabb);
    }
}
#endif //ENGINE23_AABBINTERSECTSPLANE_H
