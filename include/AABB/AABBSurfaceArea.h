//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBSURFACEAREA_H
#define ENGINE23_AABBSURFACEAREA_H

#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    T SurfaceArea(const AABB<T, N> &aabb) {
        T area = 0;
        for (size_t i = 0; i < N; ++i) {
            T side = 1;
            for (size_t j = 0; j < N; ++j) {
                if (i == j) {
                    side *= aabb.max[j] - aabb.min[j];
                } else {
                    side *= 2 * (aabb.max[j] - aabb.min[j]);
                }
            }
            area += side;
        }
        return area;
    }

}

#endif //ENGINE23_AABBSURFACEAREA_H
