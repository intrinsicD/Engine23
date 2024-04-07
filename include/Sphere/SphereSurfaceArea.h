//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERESURFACEAREA_H
#define ENGINE23_SPHERESURFACEAREA_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    T SurfaceArea(const Sphere<T, N> &sphere) {
        return 2 * std::sqrt(std::pow(M_PI, sphere.dims())) * std::pow(sphere.radius, sphere.dims() - 1) /
               tgamma(sphere.dims() / 2.0);
    }

    template<typename T>
    T SurfaceArea(const Sphere<T, 2> &circle) {
        return 2 * M_PI * circle.radius;
    }

    template<typename T>
    T SurfaceArea(const Sphere<T, 3> &sphere) {
        return 4 * M_PI * std::pow(sphere.radius, 2);
    }
}

#endif //ENGINE23_SPHERESURFACEAREA_H
