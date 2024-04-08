//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLECLOSESTPOINT_H
#define ENGINE23_TRIANGLECLOSESTPOINT_H

#include "TriangleBarycentricCoordinates.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Matrix<T, N, 1> TriangleClosestPoint(const Triangle<T, N> &triangle, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, 3> bc = TriangleBarycentricCoordinates(triangle, point);
        bc = bc.cwiseMax(Eigen::Vector<T, 3>::Zero()).cwiseMin(Eigen::Vector<T, 3>::Ones());
        return TrianglePointFromBarycentricCoordinates(triangle, bc);
    }
}

#endif //ENGINE23_TRIANGLECLOSESTPOINT_H
