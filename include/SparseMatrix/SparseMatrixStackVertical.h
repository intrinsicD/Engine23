//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPARSEMATRIXSTACKVERTICAL_H
#define ENGINE23_SPARSEMATRIXSTACKVERTICAL_H

#include "Eigen/Sparse"

namespace Bcg {
    template<typename T>
    Eigen::SparseMatrix<T> SparseMatrixStackVertical(const Eigen::SparseMatrix<T> &A, const Eigen::SparseMatrix<T> &B) {
        Eigen::SparseMatrix<T> C(A.rows() + B.rows(), A.cols());
        C.reserve(A.nonZeros() + B.nonZeros());
        for (int k = 0; k < A.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(A, k); it; ++it) {
                C.insert(it.row(), it.col()) = it.value();
            }
        }
        for (int k = 0; k < B.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(B, k); it; ++it) {
                C.insert(it.row() + A.rows(), it.col()) = it.value();
            }
        }
        C.makeCompressed();
        return C;
    }
}

#endif //ENGINE23_SPARSEMATRIXSTACKVERTICAL_H
