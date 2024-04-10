//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAYCLOSESTPOINT_H
#define ENGINE23_RAYCLOSESTPOINT_H

#include "Ray.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Matrix<T, N, 1> ClosestPoint(const Ray<T, N> &ray, const Eigen::Matrix<T, N, 1> &point) {
        Eigen::Vector<T, N> w = point - ray.origin;
        double t = w.dot(ray.direction);
        return ray.point(t);
    }

    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> ClosestPoints(const Ray<T, N> &ray1, const Ray<T, N> &ray2) {
        Eigen::Vector<T, N> w0 = ray1.origin - ray2.origin;
        T a = ray1.direction.dot(ray1.direction);
        T b = ray1.direction.dot(ray2.direction);
        T c = ray2.direction.dot(ray2.direction);
        T d = ray1.direction.dot(w0);
        T e = ray2.direction.dot(w0);
        T denominator = a * c - b * b;

        // Check if rays are parallel
        if (std::abs(denominator) < std::numeric_limits<T>::epsilon()) {
            // Handle parallel case, e.g., return one point on ray1 and the corresponding closest point on ray2
            T t2 = (b > c ? d / b : e / c); // Choose a division that avoids division by a small c
            return {ray1.point(0), ray2.point(t2)}; // Arbitrarily choosing t1 = 0 for ray1's point
        }

        T t1 = (b * e - c * d) / denominator;
        T t2 = (a * e - b * d) / denominator;

        return {ray1.point(t1), ray2.point(t2)};
    }
}

#endif //ENGINE23_RAYCLOSESTPOINT_H
