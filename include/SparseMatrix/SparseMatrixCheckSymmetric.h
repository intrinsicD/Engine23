//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPARSEMATRIXCHECKSYMMETRIC_H
#define ENGINE23_SPARSEMATRIXCHECKSYMMETRIC_H

#include "Eigen/Sparse"

namespace Bcg{
    template<typename T>
    bool SparseMatrixCheckSymmetric(const Eigen::SparseMatrix<T> &A) {
        if (A.rows() != A.cols()) {
            return false;
        }
        for (int k = 0; k < A.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(A, k); it; ++it) {
                if (A.coeff(it.col(), it.row()) != it.value()) {
                    return false;
                }
            }
        }
        return true;
    }
}
#endif //ENGINE23_SPARSEMATRIXCHECKSYMMETRIC_H
