//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERECLOSESTPOINT_H
#define ENGINE23_SPHERECLOSESTPOINT_H

#include "Sphere.h"

namespace Bcg{
    template<typename T, int N>
    Eigen::Matrix<T, N, 1> ClosestPoint(const Eigen::Matrix<T, N, 1> &point, const Sphere<T, N> &sphere) {
        Eigen::Matrix<T, N, 1> result = sphere.center + sphere.radius * (point - sphere.center).normalized();
        return result;
    }

    template<typename T, int N>
    Eigen::Matrix<T, N, 1> ClosestPoint(const Sphere<T, N> &sphere, const Eigen::Matrix<T, N, 1> &point) {
        return ClosestPoint(point, sphere);
    }
}

#endif //ENGINE23_SPHERECLOSESTPOINT_H
