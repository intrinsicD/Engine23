//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H
#define ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H

#include "Triangle.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, 3>
    TriangleBarycentricCoordinates(const Triangle<T, N> &triangle, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, N> v0 = triangle.points.col(1) - triangle.points.col(0);
        Eigen::Vector<T, N> v1 = -triangle.points.col(0) + triangle.points.col(2);
        Eigen::Vector<T, N> v2 = point - triangle.points.col(0);

        T d00 = v0.dot(v0);
        T d01 = v0.dot(v1);
        T d11 = v1.dot(v1);
        T d20 = v2.dot(v0);
        T d21 = v2.dot(v1);

        T denom = d00 * d11 - d01 * d01;

        if (denom < std::numeric_limits<T>::epsilon()) {
            return Eigen::Vector<T, 3>::Zero();
        }

        Eigen::Vector<T, 3> result;
        result[1] = (d11 * d20 - d01 * d21) / denom;
        result[2] = (d00 * d21 - d01 * d20) / denom;
        result[0] = 1.0 - result[1] - result[2];

        return result;
    }

    template<typename T, int N>
    Eigen::Vector<T, N> TrianglePointFromBarycentricCoordinates(const Triangle<T, N> &triangle,
                                                                const Eigen::Vector<T, 3> &barycentric_coordinates) {
        return triangle.points.col(0) * barycentric_coordinates[0] +
               triangle.points.col(1) * barycentric_coordinates[1] +
               triangle.points.col(2) * barycentric_coordinates[2];
    }
}

#endif //ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H
