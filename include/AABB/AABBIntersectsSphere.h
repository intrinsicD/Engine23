//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBINTERSECTSSPHERE_H
#define ENGINE23_AABBINTERSECTSSPHERE_H

#include "SphereIntersectsAABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const AABB<T, N> &aabb, const Sphere<T, N> &sphere) {
        return Intersects(sphere, aabb);
    }
}

#endif //ENGINE23_AABBINTERSECTSSPHERE_H
