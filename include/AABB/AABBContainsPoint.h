//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBCONTAINSPOINT_H
#define ENGINE23_AABBCONTAINSPOINT_H

#include "AABB.h"

namespace Bcg {
    template<typename T, int N>
    bool Contains(const AABB<T, N> &aabb, const Eigen::Vector<T, N> &point) {
        for (size_t i = 0; i < N; ++i) {
            if (point[i] < aabb.min[i] || point[i] > aabb.max[i]) {
                return false;
            }
        }
        return true;
    }
}

#endif //ENGINE23_AABBCONTAINSPOINT_H
