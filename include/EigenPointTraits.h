//
// Created by alex on 20.06.24.
//

#ifndef ENGINE23_EIGENPOINTTRAITS_H
#define ENGINE23_EIGENPOINTTRAITS_H

#include "PointTraits.h"
#include "Eigen/Core"

namespace Bcg {
    namespace traits {

        // Generic scalar_type specialization for Eigen Objects
        template<typename Derived>
        struct scalar_type<Eigen::DenseBase<Derived>> {
            using type = typename Eigen::DenseBase<Derived>::Scalar;
        };

        // Generic access specialization for Eigen::Vector
        template<typename T>
        struct access<Eigen::Vector<T, 3>, 0> {
            static auto get(const Eigen::Vector<T, 3> &p) {
                return p.x();
            }
        };

        template<typename T>
        struct access<Eigen::Vector<T, 3>, 1> {
            static auto get(const Eigen::Vector<T, 3> &p) {
                return p.y();
            }
        };

        template<typename T>
        struct access<Eigen::Vector<T, 3>, 2> {
            static auto get(const Eigen::Vector<T, 3> &p) {
                return p.z();
            }
        };

    }  // namespace traits
}

#endif //ENGINE23_EIGENPOINTTRAITS_H
