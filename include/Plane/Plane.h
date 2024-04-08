//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_PLANE_H
#define ENGINE23_PLANE_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T, int N>
    class Plane {
    public:
        Eigen::Vector<T, N> normal;
        T d;

        Plane() : normal(Eigen::Vector<T, N>::Unit(0)), d(0) {}

        Plane(const Eigen::Vector<T, N> &normal, T d) : normal(normal.normalized()), d(d) {}

        Plane(const Eigen::Vector<T, N> &normal, const Eigen::Vector<T, N> &point) : normal(normal.normalized()) {
            d = -normal.dot(point);
        }

        Eigen::Vector<T, N> point() const {
            return -d * normal;
        }

        bool operator==(const Plane &plane) const {
            return normal.isApprox(plane.normal) && d == plane.d;
        }

        bool operator!=(const Plane &plane) const {
            return !operator==(plane);
        }

        bool operator<(const Plane &plane) const {
            if (normal < plane.normal) {
                return true;
            }

            if (normal > plane.normal) {
                return false;
            }

            return d < plane.d;
        }

        bool operator<=(const Plane &plane) const {
            return !plane.operator<(*this);
        }

        bool operator>(const Plane &plane) const {
            return plane.operator<(*this);
        }
    };
}

#endif //ENGINE23_PLANE_H
