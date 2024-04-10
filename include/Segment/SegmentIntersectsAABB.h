//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENTINTERSECTSAABB_H
#define ENGINE23_SEGMENTINTERSECTSAABB_H

#include "Segment.h"
#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const Segment<T, N> &segment, const AABB<T, N> &aabb) {
        Eigen::Matrix<T, N, 1> dir = segment.end - segment.start;
        T tmin = 0;
        T tmax = 1;
        for (size_t i = 0; i < N; ++i) {
            if (dir[i] == 0) {
                if (segment.start[i] < aabb.min[i] || segment.start[i] > aabb.max[i]) {
                    return false;
                }
            } else {
                T t1 = (aabb.min[i] - segment.start[i]) / dir[i];
                T t2 = (aabb.max[i] - segment.start[i]) / dir[i];
                T tmin_ = std::min(t1, t2);
                T tmax_ = std::max(t1, t2);
                tmin = std::max(tmin, tmin_);
                tmax = std::min(tmax, tmax_);
                if (tmin > tmax) {
                    return false;
                }
            }
        }
        return true;
    }
}

#endif //ENGINE23_SEGMENTINTERSECTSAABB_H
