//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPHEREINTERSECTSLINE_H
#define ENGINE23_SPHEREINTERSECTSLINE_H

#include "LineIntersectsSphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Sphere<T, N> &sphere, const Line<T, N> &line) {
        return Intersects(line, sphere);
    }

}

#endif //ENGINE23_SPHEREINTERSECTSLINE_H
