//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANECLOSESTPOINT_H
#define ENGINE23_PLANECLOSESTPOINT_H

#include "Plane.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Matrix<T, N, 1> PlaneClosestPoint(const Plane<T, N> &plane, const Eigen::Vector<T, N> &point) {
        return point - plane.normal * (plane.normal.dot(point) + plane.d);
    }
}

#endif //ENGINE23_PLANECLOSESTPOINT_H
