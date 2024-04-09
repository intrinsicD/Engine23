//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H
#define ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H

#include "Triangle.h"
#include "TriangleArea.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, 3>
    ToBarycentricCoordinates(const Triangle<T, N> &triangle, const Eigen::Vector<T, N> &point) {
        //project the point onto the plane of the triangle
        double area = TriangleArea(triangle);
        double lamnda0 = TriangleArea(Triangle<T, N>(point, triangle.points.row(1), triangle.points.row(2))) / area;
        double lamnda1 = TriangleArea(Triangle<T, N>(triangle.points.row(0), point, triangle.points.row(2))) / area;
        double lamnda2 = TriangleArea(Triangle<T, N>(triangle.points.row(0), triangle.points.row(1), point)) / area;
        return Eigen::Vector<T, 3>(lamnda0, lamnda1, lamnda2);
    }

    template<typename T, int N>
    Eigen::Vector<T, N> FromBarycentricCoordinates(const Triangle<T, N> &triangle,
                                                   const Eigen::Vector<T, 3> &barycentric_coordinates) {
        return triangle.points.transpose() * barycentric_coordinates;
    }
}

#endif //ENGINE23_TRIANGLEBARYCENTRICCOORDINATES_H
