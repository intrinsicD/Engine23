//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERELEASTSQUARESFIT_H
#define ENGINE23_SPHERELEASTSQUARESFIT_H

#include "Sphere.h"
#include "Eigen/Dense"
#include "Eigen/Cholesky"

namespace Bcg {
    template<typename T, int N>
    Sphere<T, N> SphereLeastSquaresFitSvd(const Eigen::Matrix<T, -1, N> &points) {
        /*
         * https://jekel.me/2015/Least-Squares-Sphere-Fit/
         * */
        Eigen::Matrix<T, -1, -1> A;
        A.resize(points.rows(), points.cols() + 1);
        A.block(0, 0, points.rows(), points.cols()) = 2.0 * points;
        A.col(points.cols()).setOnes();
        Eigen::Vector<T, -1> f = points.rowwise().squaredNorm();
        Eigen::Vector<T, -1> c = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(f);
        return {c.head(N), std::sqrt(c[N] + c.head(N).squaredNorm())};
    }

    template<typename T, int N>
    Sphere<T, N> SphereLeastSquaresFitFast(const Eigen::Matrix<T, -1, N> &points) {
        //Paper: Fast Geometric Fit Algorithm for Sphere Using Exact Solution
        auto x = points.col(0);
        auto y = points.col(1);
        auto z = points.col(2);

        int num_points = x.size();
        T Sx = x.sum();
        T Sy = y.sum();
        T Sz = z.sum();
        T Sxx = x.array().square().sum();
        T Syy = y.array().square().sum();
        T Szz = z.array().square().sum();
        T Sxy = (x.array() * y.array()).sum();
        T Sxz = (x.array() * z.array()).sum();
        T Syz = (y.array() * z.array()).sum();
        T Sxxx = (x.array().square() * x.array()).sum();
        T Syyy = (y.array().square() * y.array()).sum();
        T Szzz = (z.array().square() * z.array()).sum();
        T Sxyy = (x.array() * y.array().square()).sum();
        T Sxzz = (x.array() * z.array().square()).sum();
        T Sxxy = (x.array().square() * y.array()).sum();
        T Sxxz = (x.array().square() * z.array()).sum();
        T Syyz = (y.array().square() * z.array()).sum();
        T Syzz = (y.array() * z.array().square()).sum();

        T A1 = Sxx + Syy + Szz;
        T a = 2 * Sx * Sx - 2 * num_points * Sxx;
        T b = 2 * Sx * Sy - 2 * num_points * Sxy;
        T c = 2 * Sx * Sz - 2 * num_points * Sxz;
        T d = -num_points * (Sxxx + Sxyy + Sxzz) + A1 * Sx;
        T e = 2 * Sx * Sy - 2 * num_points * Sxy;
        T f = 2 * Sy * Sy - 2 * num_points * Syy;
        T g = 2 * Sy * Sz - 2 * num_points * Syz;
        T h = -num_points * (Sxxy + Syyy + Syzz) + A1 * Sy;
        T j = 2 * Sx * Sz - 2 * num_points * Sxz;
        T k = 2 * Sy * Sz - 2 * num_points * Syz;
        T l = 2 * Sz * Sz - 2 * num_points * Szz;
        T m = -num_points * (Sxxz + Syyz + Szzz) + A1 * Sz;

        T delta = a * (f * l - g * k) - e * (b * l - c * k) + j * (b * g - c * f);
        T xc = (d * (f * l - g * k) - h * (b * l - c * k) + m * (b * g - c * f)) / delta;
        T yc = (a * (h * l - m * g) - e * (d * l - m * c) + j * (d * g - h * c)) / delta;
        T zc = (a * (f * m - h * k) - e * (b * m - d * k) + j * (b * h - d * f)) / delta;
        T R = sqrt(xc * xc + yc * yc + zc * zc + (A1 - 2 * (xc * Sx + yc * Sy + zc * Sz)) / num_points);
        return Sphere < T, N > ({ xc, yc, zc }, R);
    }
}

#endif //ENGINE23_SPHERELEASTSQUARESFIT_H
