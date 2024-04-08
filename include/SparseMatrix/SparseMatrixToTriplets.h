//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_SPARSEMATRIXTOTRIPLETS_H
#define ENGINE23_SPARSEMATRIXTOTRIPLETS_H

#include "Eigen/Sparse"

namespace Bcg {
    template<typename T>
    std::vector<Eigen::Triplet<T>> SparseMatrixToTriplets(const Eigen::SparseMatrix<T> &A) {
        std::vector<Eigen::Triplet<T>> triplets;
        triplets.reserve(A.nonZeros());

        for (int k = 0; k < A.outerSize(); ++k) {
            for (typename Eigen::SparseMatrix<T>::InnerIterator it(A, k); it; ++it) {
                triplets.emplace_back(it.row(), it.col(), it.value());
            }
        }
        return triplets;
    }
}

#endif //ENGINE23_SPARSEMATRIXTOTRIPLETS_H
