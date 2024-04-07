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
        Eigen::Vector<T, N> result = center;
        for (size_t j = 0; j < N; ++j) {
            if (i & (1 << j)) {
                result[j] += halfSize[j];
            } else {
                result[j] -= halfSize[j];
            }
        }
        return result;
    }

    template<typename T, int N>
    AABB<T, N> MortonChildAABB(const AABB<T, N> &aabb, size_t i) {
        assert(i < (1 << N));
        Eigen::Vector<T, N> center = aabb.Center();
        Eigen::Vector<T, N> halfSize = aabb.HalfSize();
        Eigen::Vector<T, N> min = center;
        Eigen::Vector<T, N> max = center;
        for (size_t j = 0; j < N; ++j) {
            if (i & (1 << j)) {
                min[j] += halfSize[j];
            } else {
                max[j] -= halfSize[j];
            }
        }
        return AABB<T, N>(min, max);
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
        return MortonChildIndex(aabb.Center(), point);
    }

    template<typename T, int N>
    std::array<AABB<T, N>, 1 << N> Subdivide(const AABB<T, N> &aabb) {
        std::array<AABB<T, N>, 1 << N> result;
        Eigen::Vector<T, N> center = aabb.Center();
        Eigen::Vector<T, N> halfSize = aabb.HalfSize();
        for (size_t i = 0; i < (1 << N); ++i) {
            Eigen::Vector<T, N> min = center;
            Eigen::Vector<T, N> max = center;
            for (size_t j = 0; j < N; ++j) {
                if (i & (1 << j)) {
                    min[j] -= halfSize[j];
                } else {
                    max[j] += halfSize[j];
                }
            }
            result[i] = AABB<T, N>(min, max);
        }
        return result;
    }

}

#endif //ENGINE23_AABBMORTONSUBDIVIDE_H
