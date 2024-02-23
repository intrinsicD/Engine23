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
        // Compute the dot product
        double dot = a.dot(b);

        // Compute the magnitudes of the vectors
        double norm_a = a.norm();
        double norm_b = b.norm();

        // Compute the cosine of the angle
        double cos_theta = dot / (norm_a * norm_b);

        return SafeAcos(cos_theta);
    }
}

#endif //ENGINE23_SAFEANGLE_H
