//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPARSEMATRIXCHECKFINITE_H
#define ENGINE23_SPARSEMATRIXCHECKFINITE_H

#include "Eigen/Sparse"

namespace Bcg {
    template<typename T>
    bool SparseMatrixCheckFinite(const Eigen::SparseMatrix<T> &A) {
        for (int k = 0; k < A.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(A, k); it; ++it) {
                if (!std::isfinite(it.value())) {
                    return false;
                }
            }
        }
        return true;
    }
}

#endif //ENGINE23_SPARSEMATRIXCHECKFINITE_H
