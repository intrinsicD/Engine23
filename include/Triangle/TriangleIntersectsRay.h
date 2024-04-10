//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_TRIANGLEINTERSECTSRAY_H
#define ENGINE23_TRIANGLEINTERSECTSRAY_H

#include "RayIntersectsTriangle.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Triangle<T, N> &triangle, const Ray<T, N> &ray) {
        return Intersects(ray, triangle);
    }
}

#endif //ENGINE23_TRIANGLEINTERSECTSRAY_H
