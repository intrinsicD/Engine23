//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHEREITERATIVEFIT_H
#define ENGINE23_SPHEREITERATIVEFIT_H

#include "Sphere.h"

namespace Bcg {
    template<typename T, int M, int N>
    Sphere<T, N> IterativeFit(const Eigen::Matrix<T, M, N> &points, size_t max_iterations = 100, T epsilon = 1e-6) {
        Eigen::Vector<T, N> average = points.colwise().mean();
        Sphere<T, N> sphere(average, 0);

        // Correctly initialize the radius to ensure all points are within the sphere
        T maxDistSqr = 0;
        for (long i = 0; i < points.rows(); ++i) {
            Eigen::Vector<T, N> diff = points.row(i) - sphere.center.transpose();
            T distSqr = diff.squaredNorm();
            maxDistSqr = std::max(maxDistSqr, distSqr);
        }
        sphere.radius = std::sqrt(maxDistSqr); // Initial radius as the distance to the farthest point


        T invNumPoints = 1.0 / static_cast<T>(points.size());
        T epsilonSqr = epsilon * epsilon;

        for (size_t iteration = 0; iteration < max_iterations; ++iteration) {
            // Update the iterates.
            Eigen::Vector<T, N> current = sphere.center;

            // Compute average L, dL/da, dL/db, dL/dc.
            T lenAverage = 0.0;
            Eigen::Vector<T, N> derLenAverage = Eigen::Vector<T, N>::Zero();
            for (long i = 0; i < points.rows(); ++i) {
                Eigen::Vector<T, N> diff = points.row(i) - sphere.center.transpose();
                T length = diff.norm();
                if (length > (T) 0) {
                    lenAverage += length;
                    T invLength = ((T) 1) / length;
                    derLenAverage -= invLength * diff;
                }
            }
            lenAverage *= invNumPoints;
            derLenAverage *= invNumPoints;

            sphere.center = average + lenAverage * derLenAverage;
            sphere.radius = lenAverage;

            Eigen::Vector<T, N> diff = sphere.center - current;
            T diffSqrLen = diff.dot(diff);
            if (diffSqrLen <= epsilonSqr) {
                break;
            }
        }
        return sphere;
    }
}

#endif //ENGINE23_SPHEREITERATIVEFIT_H
