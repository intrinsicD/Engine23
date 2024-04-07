//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBCLOSESTPOINT_H
#define ENGINE23_AABBCLOSESTPOINT_H

#include "AABB.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const Eigen::Vector<T, N> &point, const AABB<T, N> &aabb) {
        Eigen::Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = std::clamp(point[i], aabb.min[i], aabb.max[i]);
        }
        return result;
    }

    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const AABB<T, N> &aabb, const Eigen::Vector<T, N> &point) {
        return ClosestPoint(point, aabb);
    }
}

#endif //ENGINE23_AABBCLOSESTPOINT_H
