//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_RAY_H
#define ENGINE23_RAY_H

#include "Eigen/Core"

namespace Bcg{
    template<typename T, int N>
    class Ray {
    public:
        Eigen::Matrix<T, N, 1> origin;
        Eigen::Matrix<T, N, 1> direction;

        Ray() : origin(Eigen::Matrix<T, N, 1>::Zero()), direction(Eigen::Matrix<T, N, 1>::Unit(0)) {}

        Ray(const Eigen::Matrix<T, N, 1> &origin, const Eigen::Matrix<T, N, 1> &direction) : origin(origin), direction(direction) {}

        Eigen::Matrix<T, N, 1> point(T t) const {
            assert(t >= 0);
            return origin + t * direction;
        }

        bool operator==(const Ray &ray) const {
            return origin == ray.origin && direction == ray.direction;
        }

        bool operator!=(const Ray &ray) const {
            return !operator==(ray);
        }

        bool operator<(const Ray &ray) const {
            if (origin < ray.origin) {
                return true;
            }

            if (origin > ray.origin) {
                return false;
            }

            return direction < ray.direction;
        }

        bool operator<=(const Ray &ray) const {
            return !ray.operator<(*this);
        }

        bool operator>(const Ray &ray) const {
            return ray.operator<(*this);
        }

        bool operator>=(const Ray &ray) const {
            return !operator<(ray);
        }
    };
}

#endif //ENGINE23_RAY_H
