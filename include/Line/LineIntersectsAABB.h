//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_LINEINTERSECTSAABB_H
#define ENGINE23_LINEINTERSECTSAABB_H

#include "Line.h"
#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const Line<T, N> &line, const AABB<T, N> &aabb) {
        Eigen::Vector<T, N> tmin;
        Eigen::Vector<T, N> tmax;
        for (int i = 0; i < N; ++i) {
            T t1 = (aabb.min[i] - line.origin[i]) / line.direction[i];
            T t2 = (aabb.max[i] - line.origin[i]) / line.direction[i];
            tmin[i] = std::min(t1, t2);
            tmax[i] = std::max(t1, t2);
        }
        T tmin_max = tmin.maxCoeff();
        T tmax_min = tmax.minCoeff();
        return tmin_max <= tmax_min;
    }
}

#endif //ENGINE23_LINEINTERSECTSAABB_H
