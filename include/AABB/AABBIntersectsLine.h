//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_AABBINTERSECTSLINE_H
#define ENGINE23_AABBINTERSECTSLINE_H

#include "LineIntersectsAABB.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const AABB<T, N> &aabb, const Line<T, N> &line) {
        return Intersects(line, aabb);
    }
}

#endif //ENGINE23_AABBINTERSECTSLINE_H
