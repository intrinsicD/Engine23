//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_AABB_H
#define ENGINE23_AABB_H

#include "Eigen/Core"
#include <limits>

namespace Bcg {
    template<typename T=double, int N = 3>
    class AABB {
    public:
        AABB() = default;

        AABB(const Eigen::Vector <T, N> &min, const Eigen::Vector <T, N> &max) : min(min), max(max) {}

        AABB &grow(const Eigen::Vector <T, N> &point) {
            min = (min, point).cwiseMin();
            max = (max, point).cwiseMax();
        }

        AABB &merge(const AABB<T, N> &other) {
            min = (min, other.min).cwiseMin();
            max = (max, other.max).cwiseMax();
        }

        [[nodiscard]] Eigen::Vector <T, N> center() const {
            return (min + max) / 2.0;
        }

        [[nodiscard]] Eigen::Vector <T, N> halfextent() const {
            return (max - min) / 2.0;
        }

        void set_center(const Eigen::Vector <T, N> &center) {
            Eigen::Vector <T, N> halfextent = this->halfextent();
            min = center - halfextent;
            max = center + halfextent;
        }

        void set_halfextent(const Eigen::Vector <T, N> &halfextent) {
            Eigen::Vector <T, N> center = this->center();
            min = center - halfextent;
            max = center + halfextent;
        }

        [[nodiscard]] consteval int num_vertices() const {
            return 1 << dims(); //2^n
        }

        [[nodiscard]] consteval int num_edges() const {
            return dims() * (1 << (dims() - 1)); //n*2^(n-1)
        }

        [[nodiscard]] consteval int num_faces() const {
            return 2 - num_vertices() + num_edges();/*2 + (dims() - 2) * (1 << (dims() - 1));*/ // 2+(n-2)*2^(n-1)
        }

        [[nodiscard]] consteval int dims() const {
            return min.size();
        }

        bool operator==(const AABB &box) const {
            return min.isApprox(box.min) && max.isApprox(box.max);
        }

        bool operator!=(const AABB &box) const {
            return !operator==(box);
        }

        bool operator<(const AABB &box) const {
            if ((min.array() < box.min.array()).all()) {
                return true;
            }

            if ((min.array() > box.min.array()).all()) {
                return false;
            }

            return (max.array() < box.max.array()).all();
        }

        bool operator<=(const AABB &box) const {
            return !box.operator<(*this);
        }

        bool operator>(const AABB &box) const {
            return box.operator<(*this);
        }

        bool operator>=(const AABB &box) const {
            return !operator<(box);
        }

        Eigen::Vector <T, N> min = Eigen::Vector<T, N>::Constant(std::numeric_limits<T>::max());
        Eigen::Vector <T, N> max = -Eigen::Vector<T, N>::Constant(std::numeric_limits<T>::max());
    };

    using AABB3 = AABB<real_t, 3>;
}

#endif //ENGINE23_AABB_H
