//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBMORTONSUBDIVIDE_H
#define ENGINE23_AABBMORTONSUBDIVIDE_H

#include "AABB.h"
#include <cassert>

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N>
    MortonChildCenter(const Eigen::Vector<T, N> &center, const Eigen::Vector<T, N> &halfSize, size_t i) {
        assert(i < (1 << N));
        Eigen::Vector<T, N> result;
        for (unsigned int j = 0, mask = 1; j < N; ++j, mask <<= (unsigned int) 1) {
            T sign = ((i & mask) ? 0.5 : -0.5);
            result[j] = center[j] + sign * halfSize[j];
        }
        return result;
    }

    template<typename T, int N>
    AABB<T, N> MortonChildAABB(const AABB<T, N> &aabb, size_t i) {
        Eigen::Vector<T, N> center = aabb.center();
        Eigen::Vector<T, N> halfSize = aabb.halfextent();
        AABB<T, N> result;
        result.set_centered_form(MortonChildCenter(center, halfSize, i), halfSize / 2.0);
        return result;
    }

    template<typename T, int N>
    size_t MortonChildIndex(const Eigen::Vector<T, N> &center, const Eigen::Vector<T, N> &point) {
        size_t result = 0;
        for (size_t i = 0; i < N; ++i) {
            if (point[i] > center[i]) {
                result |= (1 << i);
            }
        }
        return result;
    }

    template<typename T, int N>
    size_t MortonChildIndex(const AABB<T, N> &aabb, const Eigen::Vector<T, N> &point) {
        return MortonChildIndex(aabb.center(), point);
    }

    template<typename T, int N>
    std::array<AABB<T, N>, 1 << N> Subdivide(const AABB<T, N> &aabb) {
        std::array<AABB<T, N>, 1 << N> result;
        Eigen::Vector<T, N> center = aabb.center();
        Eigen::Vector<T, N> halfSize = aabb.halfextent();
        for (size_t i = 0; i < (1 << N); ++i) {
            result[i].set_centered_form(MortonChildCenter(center, halfSize, i), halfSize / 2.0);
        }
        return result;
    }

}

#endif //ENGINE23_AABBMORTONSUBDIVIDE_H
