//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANEINTERSECTSLINE_H
#define ENGINE23_PLANEINTERSECTSLINE_H

#include "Plane.h"
#include "Line.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Plane<T, N> &plane, const Line<T, N> &line) {
        return plane.normal.dot(line.direction) != 0;
    }
}

#endif //ENGINE23_PLANEINTERSECTSLINE_H
