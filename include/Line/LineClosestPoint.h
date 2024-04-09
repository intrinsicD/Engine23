//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_LINECLOSESTPOINT_H
#define ENGINE23_LINECLOSESTPOINT_H

#include "Line.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const Line<T, N> &line, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, N> w = point - line.origin;
        double t = (w).dot(line.direction);
        return line.point(t);
    }

    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> ClosestPoint(const Line<T, N> &line1, const Line<T, N> &line2) {
        Eigen::Vector<T, N> w0 = line1.origin - line2.origin;
        T a = line1.direction.dot(line1.direction);
        T b = line1.direction.dot(line2.direction);
        T c = line2.direction.dot(line2.direction);
        T d = line1.direction.dot(w0);
        T e = line2.direction.dot(w0);
        T denominator = a * c - b * b;

        // Check if lines are parallel
        if (std::abs(denominator) < std::numeric_limits<T>::epsilon()) {
            std::cerr << "Lines are parallel or nearly parallel" << std::endl;
            // Handle parallel case, e.g., return one point on line1 and the corresponding closest point on line2
            T t2 = (b > c ? d / b : e / c); // Choose a division that avoids division by a small c
            return {line1.point(0), line2.point(t2)}; // Arbitrarily choosing t1 = 0 for line1's point
        }

        T t1 = (b * e - c * d) / denominator;
        T t2 = (a * e - b * d) / denominator;

        return {line1.point(t1), line2.point(t2)};
    }
}

#endif //ENGINE23_LINECLOSESTPOINT_H
