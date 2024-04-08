//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANELEASTSQUARESFIT_H
#define ENGINE23_PLANELEASTSQUARESFIT_H

#include "Plane.h"
#include "Eigen/Eigenvalues"

namespace Bcg {
    template<typename T, int M, int N>
    Plane<T, N> PlaneLeastSquaresFit(const Eigen::Matrix<T, M, N> &points) {
        Eigen::Vector<T, N> centroid = points.colwise().mean();
        Eigen::Matrix<T, M, N> centered = points.rowwise() - centroid.transpose();
        Eigen::Matrix<T, N, N> cov = centered.transpose() * centered / (points.rows() - 1);
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix<T, N, N>> eig(cov);
        Eigen::Vector<T, N> normal = eig.eigenvectors().col(0);
        return Plane<T, N>(normal, -normal.dot(centroid));
    }
}

#endif //ENGINE23_PLANELEASTSQUARESFIT_H
