//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_AABBSURFACEAREA_H
#define ENGINE23_AABBSURFACEAREA_H

#include "AABB.h"

namespace Bcg {
    template<typename T, int N>
    T SurfaceArea(const AABB<T, N> &aabb) {
        auto extent = aabb.max - aabb.min;
        T area = 2 * extent[0] * extent[1];
        area += 2 * extent[1] * extent[2];
        area += 2 * extent[0] * extent[2];
        return area;
    }

}

#endif //ENGINE23_AABBSURFACEAREA_H
