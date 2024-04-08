//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_MATRIXSTACKHORIZONTAL_H
#define ENGINE23_MATRIXSTACKHORIZONTAL_H

#include "Eigen/Core"

namespace Bcg{
    template<typename Derived>
    Eigen::Matrix<typename Derived::Scalar, Eigen::Dynamic, Eigen::Dynamic>
    MatrixStackHorizontal(const Eigen::MatrixBase<Derived> &matrix1, const Eigen::MatrixBase<Derived> &matrix2) {
        Eigen::Matrix<typename Derived::Scalar, Eigen::Dynamic, Eigen::Dynamic> result(matrix1.rows(), matrix1.cols() + matrix2.cols());
        result << matrix1, matrix2;
        return result;
    }
}
#endif //ENGINE23_MATRIXSTACKHORIZONTAL_H
