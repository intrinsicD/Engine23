//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERE_H
#define ENGINE23_SPHERE_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T, int N>
    class Sphere {
    public:
        Eigen::Matrix<T, N, 1> center;
        T radius;

        Sphere() : center(Eigen::Matrix<T, N, 1>::Zero()), radius(0) {}

        Sphere(const Eigen::Matrix<T, N, 1> &center, T radius) : center(center), radius(radius) {}

        Sphere(const Sphere<T, N> &other) : center(other.center), radius(other.radius) {}

        [[nodiscard]] consteval size_t dims() const {
            return (N > 0 ? N : center.size());
        }

        Sphere<T, N> &operator=(const Sphere<T, N> &other) {
            center = other.center;
            radius = other.radius;
            return *this;
        }

        bool operator==(const Sphere<T, N> &other) const {
            return center.isApprox(other.center) && radius == other.radius;
        }

        bool operator!=(const Sphere<T, N> &other) const {
            return center != other.center || radius != other.radius;
        }
    };
}

#endif //ENGINE23_SPHERE_H
