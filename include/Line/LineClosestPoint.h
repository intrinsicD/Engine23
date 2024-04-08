//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_LINECLOSESTPOINT_H
#define ENGINE23_LINECLOSESTPOINT_H

#include "Line.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> LineClosestPoint(const Line<T, N> &line, const Eigen::Vector<T, N> &point) {
        return line.origin + line.direction * (point - line.origin).dot(line.direction) / line.direction.squaredNorm();
    }

    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> LineClosestPoints(const Line<T, N> &line1, const Line<T, N> &line2) {
        Eigen::Vector<T, N> w0 = line1.origin - line2.origin;
        double a = line1.direction.squaredNorm();
        double b = line1.direction.dot(line2.direction);
        double c = line2.direction.squaredNorm();
        double d = line1.direction.dot(w0);
        double e = line2.direction.dot(w0);
        double denominator = a * c - b * b;
        double t1 = (b * e - c * d) / denominator;
        double t2 = (a * e - b * d) / denominator;
        return {line1.point(t1), line2.point(t2)};
    }
}

#endif //ENGINE23_LINECLOSESTPOINT_H
