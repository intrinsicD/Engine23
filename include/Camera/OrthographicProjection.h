//
// Created by alex on 29.05.24.
//

#ifndef ENGINE23_ORTHOGRAPHICPROJECTION_H
#define ENGINE23_ORTHOGRAPHICPROJECTION_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T>
    struct OrhographicParameters {
        T near, far, top, bottom, left, right;
    };

    template<typename T>
    Eigen::Matrix<T, 4, 4> OrthographicProjectionMatrix(T near, T far, T top, T bottom, T left, T right) {
        Eigen::Matrix<T, 4, 4> matrix = Eigen::Matrix<T, 4, 4>::Identity();
        T rml = right - left;
        T tmb = top - bottom;
        T fmn = far - near;
        matrix(0, 0) = 2 / rml;
        matrix(1, 1) = 2 / tmb;
        matrix(2, 2) = -2 / fmn;
        matrix(0, 3) = -(right + left) / rml;
        matrix(1, 3) = -(top + bottom) / tmb;
        matrix(2, 3) = -(far + near) / fmn;
        matrix(3, 3) = 1;
        return matrix;
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> OrthographicProjectionMatrix(const OrhographicParameters<T> &parameters) {
        return OrthographicProjectionMatrix(parameters.near, parameters.far,
                                            parameters.top, parameters.bottom,
                                            parameters.left, parameters.right);
    }

    template<typename T>
    OrhographicParameters<T> OrhographicParametersFromMatrix(const Eigen::Matrix<T, 4, 4> &matrix) {
        OrhographicParameters<T> parameters;
        // Extract near and far
        parameters.near = (matrix(2, 3) + 1) / matrix(2, 2);
        parameters.far = (matrix(2, 3) - 1) / matrix(2, 2);

        // Extract left and right
        parameters.left = -(1 + matrix(0, 3)) / matrix(0, 0);
        parameters.right = (1 - matrix(0, 3)) / matrix(0, 0);

        // Extract top and bottom
        parameters.bottom = -(1 + matrix(1, 3)) / matrix(1, 1);
        parameters.top = (1 - matrix(1, 3)) / matrix(1, 1);
        return parameters;
    }


}

#endif //ENGINE23_ORTHOGRAPHICPROJECTION_H
