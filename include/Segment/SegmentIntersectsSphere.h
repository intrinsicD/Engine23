//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENTINTERSECTSSPHERE_H
#define ENGINE23_SEGMENTINTERSECTSSPHERE_H

#include "Segment.h"
#include "Sphere.h"

namespace Bcg{
    template<typename T, int N>
    bool Intersects(const Segment<T, N> &segment, const Sphere<T, N> &sphere) {
        Eigen::Matrix<T, N, 1> m = segment.start - sphere.center;
        Eigen::Matrix<T, N, 1> d = segment.end - segment.start;
        T a = d.dot(d);
        T b = 2 * m.dot(d);
        T c = m.dot(m) - sphere.radius * sphere.radius;
        T disc = b * b - 4 * a * c;
        if (disc < 0) {
            return false;
        }
        disc = std::sqrt(disc);
        T t1 = (-b - disc) / (2 * a);
        T t2 = (-b + disc) / (2 * a);
        if (t1 >= 0 && t1 <= 1) {
            return true;
        }
        if (t2 >= 0 && t2 <= 1) {
            return true;
        }
        return false;
    }
}

#endif //ENGINE23_SEGMENTINTERSECTSSPHERE_H
