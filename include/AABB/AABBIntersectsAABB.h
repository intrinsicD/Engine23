//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBINTERSECTSAABB_H
#define ENGINE23_AABBINTERSECTSAABB_H

#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const AABB<T, N> &aabb, const AABB<T, N> &other) {
        for (size_t i = 0; i < N; ++i) {
            if (aabb.min[i] > other.max[i] || aabb.max[i] < other.min[i]) {
                return false;
            }
        }
        return true;
    }

}
#endif //ENGINE23_AABBINTERSECTSAABB_H
