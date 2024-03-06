//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_SAFEANGLE_H
#define ENGINE23_SAFEANGLE_H

#include "Eigen/Core"
#include "SafeAcos.h"

namespace Bcg{
    template<typename T, int N>
    double SafeAngle(const Eigen::Vector<T, N> &a, const Eigen::Vector<T, N> &b){
        return SafeAcos(a.dot(b) / (std::max(a.norm(), 1e-8) * std::max(b.norm(), 1e-8)));
    }
}

#endif //ENGINE23_SAFEANGLE_H
