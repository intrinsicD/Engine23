//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERELEASTSQUARESFIT_H
#define ENGINE23_SPHERELEASTSQUARESFIT_H

#include "Sphere.h"
#include "Eigen/Dense"
#include "Eigen/Cholesky"

namespace Bcg{
    template<typename T, int M, int N>
    bool SphereLeastSquaresFitSmallData(const Eigen::Matrix<T, M, N> &points, Sphere<T, N> &sphere) {
        // Compute the average of the data points.
        Eigen::Vector<T, N> average = points.colwise().mean();

        // Compute the Y matrix (points - average) and S matrix (element-wise squared Y)
        Eigen::Matrix<T, M, N> Y = points.rowwise() - average.transpose();
        Eigen::Matrix<T, M, N> S = Y.array().square();

        // Compute the covariance matrix cov of the Y
        Eigen::Matrix<T, N, N> cov = (Y.adjoint() * Y) / points.rows();

        // Compute the right-hand side R of the linear system M*(C-A) = R.
        Eigen::Vector<T, N> R = (Y.array() * S.array()).matrix().colwise().sum() * 0.5;

        // Solve the linear system M*(C-A) = R for the center C.
        Eigen::LDLT<Eigen::Matrix<T, N, N>> ldlt(cov);
        Eigen::Vector<T, N> C_minus_A = ldlt.solve(R);
        sphere.center = average + C_minus_A;

        // Compute the average squared distance and the radius
        T avg_squared_dist = (S.rowwise().sum().mean());
        sphere.radius = std::sqrt(avg_squared_dist);

        return cov.determinant() != 0;
    }

    template<typename T, int N>
    Sphere<T, N> SphereLeastSquaresFitLargeData(const Eigen::Matrix<T, -1, N> &points) {
        /*
         * https://jekel.me/2015/Least-Squares-Sphere-Fit/
         * */
        Eigen::Matrix<T, -1, -1> A;
        A.resize(points.rows(), points.cols() + 1);
        A.block(0, 0, points.rows(), points.cols()) = 2.0 * points;
        A.col(points.cols()).setOnes();
        Eigen::Vector<T, -1> f = points.colwise().squaredNorm();
        Eigen::Vector<T, -1> c = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(f);
        return {c.head(N), std::sqrt(c[N] + c.head(N).squaredNorm())};
    }
}

#endif //ENGINE23_SPHERELEASTSQUARESFIT_H
