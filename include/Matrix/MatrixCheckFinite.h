//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_MATRIXCHECKFINITE_H
#define ENGINE23_MATRIXCHECKFINITE_H

#include "Eigen/Core"

namespace Bcg {
    template<typename Derived>
    bool MatrixCheckFinite(const Eigen::MatrixBase<Derived> &matrix) {
        return matrix.allFinite();
    }
}

#endif //ENGINE23_MATRIXCHECKFINITE_H
