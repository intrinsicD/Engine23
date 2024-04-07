//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERELEASTSQUARESFIT_H
#define ENGINE23_SPHERELEASTSQUARESFIT_H

#include "Sphere.h"

namespace Bcg{
    template<typename Derived>
    bool SphereLeastSquaresFitSmallData(const Eigen::EigenBase<Derived> &points, Sphere<typename Derived::Scalar, 3> &sphere) {
        using T = typename Derived::Scalar;
        // Compute the average of the data points.
        Eigen::Vector<T, 3> average = points.derived().colwise().mean();

        // Compute the Y matrix (points - average) and S matrix (element-wise squared Y)
        Eigen::Matrix<T, Eigen::Dynamic, 3> Y = points.derived().rowwise() - average.transpose();
        Eigen::Matrix<T, Eigen::Dynamic, 3> S = Y.array().square();

        // Compute the covariance matrix M of the Y
        Eigen::Matrix<T, 3, 3> M = (Y.adjoint() * Y) / points.rows();

        // Compute the right-hand side R of the linear system M*(C-A) = R.
        Eigen::Matrix<T, 3, 1> R = (Y.array() * S.array()).matrix().colwise().sum() * 0.5;

        // Solve the linear system M*(C-A) = R for the center C.
        Eigen::Vector<T, 3> C_minus_A = M.ldlt().solve(R);
        sphere.center = average + C_minus_A;

        // Compute the average squared distance and the radius
        T avg_squared_dist = (S.rowwise().sum().mean());
        sphere.radius = std::sqrt(avg_squared_dist);

        return M.determinant() != 0;
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
