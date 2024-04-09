//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLECLOSESTPOINT_H
#define ENGINE23_TRIANGLECLOSESTPOINT_H

#include "TriangleBarycentricCoordinates.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const Triangle<T, N> &triangle, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, N> normal = triangle.normal();
        Eigen::Vector<T, N> projected_point = point - normal * normal.dot(point.transpose() - triangle.points.row(0));
        Eigen::Vector<T, 3> bc = ToBarycentricCoordinates(triangle, projected_point);
        bc = bc.cwiseMax(Eigen::Vector<T, 3>::Zero()).cwiseMin(Eigen::Vector<T, 3>::Ones());
        return FromBarycentricCoordinates(triangle, bc);
    }
}

#endif //ENGINE23_TRIANGLECLOSESTPOINT_H
