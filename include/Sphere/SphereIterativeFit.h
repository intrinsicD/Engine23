//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHEREITERATIVEFIT_H
#define ENGINE23_SPHEREITERATIVEFIT_H

#include "Sphere.h"

namespace Bcg{
    template<typename T, int N>
    Sphere<T, N> IterativeFit(const T *points, size_t num_points, size_t max_iterations = 100, T epsilon = 1e-6) {
        Sphere<T, N> sphere;
        sphere.center.setZero();
        sphere.radius = 0;
        for (size_t i = 0; i < num_points; ++i) {
            sphere.center += points[i];
        }
        sphere.center /= num_points;
        for (size_t i = 0; i < num_points; ++i) {
            sphere.radius = std::max(sphere.radius, (points[i] - sphere.center).norm());
        }
        for (size_t iteration = 0; iteration < max_iterations; ++iteration) {
            T radius = 0;
            Eigen::Matrix<T, N, 1> center = Eigen::Matrix<T, N, 1>::Zero();
            for (size_t i = 0; i < num_points; ++i) {
                Eigen::Matrix<T, N, 1> diff = points[i] - sphere.center;
                T norm = diff.norm();
                if (norm > 0) {
                    center += diff * (1 - sphere.radius / norm);
                    radius += norm;
                }
            }
            if (radius < epsilon) {
                break;
            }
            sphere.center += center / num_points;
            sphere.radius = radius / num_points;
        }
        return sphere;
    }

}

#endif //ENGINE23_SPHEREITERATIVEFIT_H
