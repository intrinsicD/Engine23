//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLEAREA_H
#define ENGINE23_TRIANGLEAREA_H

#include "Triangle.h"

namespace Bcg {
    inline double AreaFromMeric(double a, double b, double c) {
        //Numerically stable herons formula for area of triangle with side lengths a, b and c.
        if (a < b) std::swap(a, b);
        if (a < c) std::swap(a, c);
        if (b < c) std::swap(b, c);

        return std::sqrt(std::abs((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)))) / 4;
    }

    template<typename T, int N>
    T TriangleArea(const Triangle<T, N> &triangle) {
        double a = (triangle.points.col(1) - triangle.points.col(0)).norm();
        double b = (triangle.points.col(2) - triangle.points.col(1)).norm();
        double c = (triangle.points.col(0) - triangle.points.col(2)).norm();
        return AreaFromMeric(a, b, c);
    }

    template<typename T, int N>
    T TriangleAreaCross(const Triangle<T, N> &triangle) {
        return (triangle.points.col(1) - triangle.points.col(0)).cross(
                triangle.points.col(2) - triangle.points.col(0)).norm() / 2;
    }
}


#endif //ENGINE23_TRIANGLEAREA_H
