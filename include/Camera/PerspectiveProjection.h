//
// Created by alex on 29.05.24.
//

#ifndef ENGINE23_PERSPECTIVEPROJECTION_H
#define ENGINE23_PERSPECTIVEPROJECTION_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T>
    struct PerspectiveASymmetricParameters {
        T near, far, top, bottom, left, right;
    };

    template<typename T>
    Eigen::Matrix<T, 4, 4> PerspectiveProjectionMatrix(T near, T far, T top, T bottom, T left, T right) {
        Eigen::Matrix<T, 4, 4> matrix = Eigen::Matrix<T, 4, 4>::Identity();
        T rml = right - left;
        T tmb = top - bottom;
        T fmn = far - near;
        T npn = near + near;
        matrix(0, 0) = npn / (rml);
        matrix(0, 2) = (right + left) / rml;
        matrix(1, 1) = npn / tmb;
        matrix(1, 2) = (top + bottom) / tmb;
        matrix(2, 2) = -(far + near) / fmn;
        matrix(2, 3) = -T(2) * far * near / fmn;
        matrix(3, 2) = -T(1);
        matrix(3, 3) = T(0);
        return matrix;
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> PerspectiveProjectionMatrix(const PerspectiveASymmetricParameters<T> &parameters) {
        return PerspectiveProjectionMatrix(parameters.near, parameters.far, parameters.top, parameters.bottom,
                                           parameters.left, parameters.right);
    }

    template<typename T>
    PerspectiveASymmetricParameters<T> PerspectiveASymmetricParametersFromMatrix(const Eigen::Matrix<T, 4, 4> &matrix) {
        PerspectiveASymmetricParameters<T> parameters;
        // Extract near and far

        parameters.near = matrix(2, 3) / (matrix(2, 2) - 1);
        parameters.far = matrix(2, 3) / (1 + matrix(2, 2));

        // Extract left and right
        parameters.left = parameters.near * (matrix(0, 2) - 1) / matrix(0, 0);
        parameters.right = parameters.near * (1 + matrix(0, 2)) / matrix(0, 0);

        // Extract top and bottom
        parameters.bottom = parameters.near * (matrix(1, 2) - 1) / matrix(1, 1);
        parameters.top = parameters.near * (1 + matrix(1, 2)) / matrix(1, 1);

        return parameters;
    }

    template<typename T>
    struct PerspectiveSymmetricParameters {
        T fovy, aspect, near, far;
    };

    template<typename T>
    PerspectiveASymmetricParameters<T>
    ConvertPerspectiveSymmetricToAsymmetricParameters(const PerspectiveSymmetricParameters<T> &parameters) {
        PerspectiveASymmetricParameters<T> result;
        result.near = parameters.near;
        result.far = parameters.far;
        result.top = parameters.near * std::tan(parameters.fovy / T(2));
        result.bottom = -result.top;
        result.left = -result.top * parameters.aspect;
        result.right = -result.left;
        return result;
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> PerspectiveProjectionMatrix(T fovy, T aspect, T near, T far) {
        Eigen::Matrix<T, 4, 4> matrix = Eigen::Matrix<T, 4, 4>::Identity();

        T tanHalfFovy = tan(fovy / 2);
        T fmn = far - near;

        matrix(0, 0) = 1 / (aspect * tanHalfFovy);
        matrix(1, 1) = 1 / (tanHalfFovy);
        matrix(2, 2) = -(far + near) / fmn;
        matrix(3, 2) = -1;
        matrix(2, 3) = -(2 * far * near) / fmn;
        return matrix;
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> PerspectiveProjectionMatrix(const PerspectiveSymmetricParameters<T> &parameters) {
        return PerspectiveProjectionMatrix(parameters.fovy, parameters.aspect, parameters.near, parameters.far);
    }

    template<typename T>
    PerspectiveSymmetricParameters<T> PerspectiveSymmetricParametersFromMatrix(const Eigen::Matrix<T, 4, 4> &matrix) {
        PerspectiveSymmetricParameters<T> parameters;
        // Extract near and far
        parameters.near = matrix(2, 3) / (matrix(2, 2) - 1);
        parameters.far = matrix(2, 3) / (1 + matrix(2, 2));

        // Extract aspect and fovy
        parameters.aspect = matrix(1, 1) / matrix(0, 0);
        parameters.fovy = 2 * atan(1 / matrix(1, 1));

        return parameters;
    }
}

#endif //ENGINE23_PERSPECTIVEPROJECTION_H
