//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLE_H
#define ENGINE23_TRIANGLE_H

#include "Eigen/Geometry"

namespace Bcg{
    template<typename T, int N>
    class Triangle {
    public:
        Eigen::Matrix<T, N, 3> points;

        Triangle() : points(Eigen::Matrix<T, N, 3>::Zero()) {}

        Triangle(const Eigen::Matrix<T, N, 3> &points) : points(points) {}

        Eigen::Matrix<T, N, 3> normal() const {
            return (points.col(1) - points.col(0)).cross(points.col(2) - points.col(0)).normalized();
        }

        bool operator==(const Triangle &triangle) const {
            return points.isApprox(triangle.points);
        }

        bool operator!=(const Triangle &triangle) const {
            return !operator==(triangle);
        }

        bool operator<(const Triangle &triangle) const {
            return points < triangle.points;
        }

        bool operator<=(const Triangle &triangle) const {
            return !triangle.operator<(*this);
        }

        bool operator>(const Triangle &triangle) const {
            return triangle.operator<(*this);
        }
    };
}

#endif //ENGINE23_TRIANGLE_H
