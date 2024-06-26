//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLEAREA_H
#define ENGINE23_TRIANGLEAREA_H

#include "Triangle.h"

namespace Bcg {
    template<typename T>
    T AreaFromMetric(T a, T b, T c) {
        //Numerically stable herons formula for area of triangle with side lengths a, b and c.
        if (a < b) std::swap(a, b);
        if (a < c) std::swap(a, c);
        if (b < c) std::swap(b, c);

        return std::sqrt(std::abs((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)))) / T(4);
    }

    template<typename T, int N>
    T TriangleArea(const Triangle<T, N> &triangle) {
        T a = (triangle.points.row(1) - triangle.points.row(0)).norm();
        T b = (triangle.points.row(2) - triangle.points.row(1)).norm();
        T c = (triangle.points.row(0) - triangle.points.row(2)).norm();
        return AreaFromMetric(a, b, c);
    }

    template<typename T, int N>
    T TriangleAreaCross(const Triangle<T, N> &triangle) {
        return (triangle.points.row(1) - triangle.points.row(0)).cross(
                triangle.points.row(2) - triangle.points.row(0)).norm() / T(2);
    }
}


#endif //ENGINE23_TRIANGLEAREA_H
