//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_ISEIGENTYPE_H
#define ENGINE23_ISEIGENTYPE_H

#include "Eigen/Core"

namespace Bcg {
    namespace Detail {
        template<typename Derived>
        constexpr bool IsEigenType_f(const Eigen::EigenBase<Derived> *) { return true; }

        constexpr bool IsEigenType_f(const void *) { return false; }
    }

    template<typename T>
    constexpr bool IsEigenType = Detail::IsEigenType_f((T *) (nullptr));
}

#endif //ENGINE23_ISEIGENTYPE_H
