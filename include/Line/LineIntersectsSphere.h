//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_LINEINTERSECTSSPHERE_H
#define ENGINE23_LINEINTERSECTSSPHERE_H

#include "Line.h"
#include "Sphere.h"

namespace Bcg {
    template<typename T, int N>
    bool Intersects(const Line<T, N> &line, const Sphere<T, N> &sphere) {
        Eigen::Vector<T, N> oc = line.origin - sphere.center;
        T a = line.direction.squaredNorm();
        T b = 2 * oc.dot(line.direction);
        T c = oc.squaredNorm() - sphere.radius * sphere.radius;
        T discriminant = b * b - 4 * a * c;
        return discriminant >= 0;
    }
}

#endif //ENGINE23_LINEINTERSECTSSPHERE_H
