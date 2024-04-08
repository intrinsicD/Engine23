//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPARSEMATRIXSTACKHORIZONTAL_H
#define ENGINE23_SPARSEMATRIXSTACKHORIZONTAL_H

#include "Eigen/Sparse"

namespace Bcg {
    template<typename T>
    Eigen::SparseMatrix<T> SparseMatrixStackHorizontal(const Eigen::SparseMatrix<T> &A, const Eigen::SparseMatrix<T> &B) {
        Eigen::SparseMatrix<T> C(A.rows(), A.cols() + B.cols());
        C.reserve(A.nonZeros() + B.nonZeros());
        for (int k = 0; k < A.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(A, k); it; ++it) {
                C.insert(it.row(), it.col()) = it.value();
            }
        }
        for (int k = 0; k < B.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(B, k); it; ++it) {
                C.insert(it.row(), it.col() + A.cols()) = it.value();
            }
        }
        C.makeCompressed();
        return C;
    }

}

#endif //ENGINE23_SPARSEMATRIXSTACKHORIZONTAL_H
