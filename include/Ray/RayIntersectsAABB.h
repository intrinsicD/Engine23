//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYINTERSECTSAABB_H
#define ENGINE23_RAYINTERSECTSAABB_H

#include "Ray.h"
#include "AABB.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Ray<T, N> &ray, const AABB<T, N> &aabb) {
        Eigen::Vector<T, N> tmin;
        Eigen::Vector<T, N> tmax;
        for (int i = 0; i < N; ++i) {
            T t1 = (aabb.min[i] - ray.origin[i]) / ray.direction[i];
            T t2 = (aabb.max[i] - ray.origin[i]) / ray.direction[i];
            tmin[i] = std::min(t1, t2);
            tmax[i] = std::max(t1, t2);
        }
        T tmin_max = tmin.maxCoeff();
        T tmax_min = tmax.minCoeff();
        return tmin_max <= tmax_min;
    }

}

#endif //ENGINE23_RAYINTERSECTSAABB_H
