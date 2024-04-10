//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_AABBINTERSECTSRAY_H
#define ENGINE23_AABBINTERSECTSRAY_H

#include "RayIntersectsAABB.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const AABB<T, N> &aabb, const Ray<T, N> &ray) {
        return Intersects(ray, aabb);
    }
}

#endif //ENGINE23_AABBINTERSECTSRAY_H
