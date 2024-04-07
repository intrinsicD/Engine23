//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBVOLUME_H
#define ENGINE23_AABBVOLUME_H

#include "AABB.h"

namespace Bcg {
    template<typename T, int N>
    T Volume(const AABB<T, N> &aabb) {
        T volume = 1;
        for (size_t i = 0; i < N; ++i) {
            volume *= aabb.max[i] - aabb.min[i];
        }
        return volume;
    }
}

#endif //ENGINE23_AABBVOLUME_H
