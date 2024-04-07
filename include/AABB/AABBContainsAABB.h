//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBCONTAINSAABB_H
#define ENGINE23_AABBCONTAINSAABB_H

#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Contains(const AABB<T, N> &aabb, const AABB<T, N> &other) {
        for (size_t i = 0; i < N; ++i) {
            if (other.min[i] < aabb.min[i] || other.max[i] > aabb.max[i]) {
                return false;
            }
        }
        return true;
    }
}

#endif //ENGINE23_AABBCONTAINSAABB_H
