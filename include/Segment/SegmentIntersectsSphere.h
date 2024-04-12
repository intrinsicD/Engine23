//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENTINTERSECTSSPHERE_H
#define ENGINE23_SEGMENTINTERSECTSSPHERE_H

#include "SegmentClosestPoint.h"
#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Segment<T, N> &segment, const Sphere<T, N> &sphere) {
        Eigen::Vector<T, N> closest = ClosestPoint(segment, sphere.center);
        T dist_sqr = (closest - sphere.center).squaredNorm();
        return dist_sqr <= sphere.radius * sphere.radius;
    }
}

#endif //ENGINE23_SEGMENTINTERSECTSSPHERE_H
