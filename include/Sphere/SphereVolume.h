//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHEREVOLUME_H
#define ENGINE23_SPHEREVOLUME_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    T Volume(const Sphere<T, N> &sphere) {
        return std::sqrt(std::pow(M_PI, sphere.dims())) * std::pow(sphere.radius, sphere.dims()) /
               tgamma(sphere.dims() / 2.0 + 1);
    }

    template<typename T>
    T Volume(const Sphere<T, 2> &circle) {
        return M_PI * std::pow(circle.radius, 2);
    }

    template<typename T>
    T Volume(const Sphere<T, 3> &sphere) {
        return (4.0 / 3.0) * M_PI * std::pow(sphere.radius, 3);
    }
}

#endif //ENGINE23_SPHEREVOLUME_H
