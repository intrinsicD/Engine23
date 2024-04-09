//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_TRIANGLE_H
#define ENGINE23_TRIANGLE_H

#include "Eigen/Geometry"
#include "Eigen/SVD"

namespace Bcg {
    template<typename T, int N>
    class Triangle {
    public:
        Eigen::Matrix<T, 3, N> points;

        Triangle() : points(Eigen::Matrix<T, N, 3>::Zero()) {}

        Triangle(const Eigen::Vector<T, N> &p0, const Eigen::Vector<T, N> &p1, const Eigen::Vector<T, N> &p2) {
            points.row(0) = p0;
            points.row(1) = p1;
            points.row(2) = p2;
        }

        explicit Triangle(const Eigen::Matrix<T, N, 3> &points) : points(points) {}

        Eigen::Vector<T, N> normal() const {
            Eigen::Matrix<T, 3, N> centered = points.rowwise() - points.colwise().mean();
            Eigen::JacobiSVD<Eigen::Matrix<T, 3, N>> svd(centered, Eigen::ComputeFullV);
            return svd.matrixV().col(points.cols() - 1);
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
