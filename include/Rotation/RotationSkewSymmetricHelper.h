//
// Created by alex on 05.03.24.
//

#ifndef ENGINE23_ROTATIONSKEWSYMMETRICHELPER_H
#define ENGINE23_ROTATIONSKEWSYMMETRICHELPER_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T>
    Eigen::Matrix<T, 3, 3> skew_symmetric_helper(const Eigen::Vector<T, 3> &vector) {
        Eigen::Matrix<T, 3, 3> matrix = Eigen::Matrix<T, 3, 3>::Zero();
        matrix(1, 0) = vector(2);
        matrix(0, 1) = -vector(2);
        matrix(0, 2) = vector(1);
        matrix(2, 0) = -vector(1);
        matrix(2, 1) = vector(0);
        matrix(1, 2) = -vector(0);
        return matrix;
    }
}

#endif //ENGINE23_ROTATIONSKEWSYMMETRICHELPER_H
