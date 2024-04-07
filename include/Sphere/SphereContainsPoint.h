//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERECONTAINSPOINT_H
#define ENGINE23_SPHERECONTAINSPOINT_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Contains(const Sphere<T, N> &sphere, const Eigen::Matrix<T, N, 1> &point) {
        return (point - sphere.center).squaredNorm() <= sphere.radius * sphere.radius;
    }
}

#endif //ENGINE23_SPHERECONTAINSPOINT_H
