//
// Created by alex on 02.01.24.
//

#ifndef ENGINE23_GLMTOEIGEN_H
#define ENGINE23_GLMTOEIGEN_H

#include "Eigen/Core"
#include "glm/glm.hpp"

namespace Bcg {
    template<typename T>
    Eigen::Matrix<T, 2, 1> GlmToEigen(const glm::vec<2, T> &glm_vec) {
        return Eigen::Matrix<T, 2, 1>(glm_vec[0], glm_vec[1]);
    }

    template<typename T>
    Eigen::Matrix<T, 3, 1> GlmToEigen(const glm::vec<3, T> &glm_vec) {
        return Eigen::Matrix<T, 3, 1>(glm_vec[0], glm_vec[1], glm_vec[2]);
    }

    template<typename T>
    Eigen::Matrix<T, 4, 1> GlmToEigen(const glm::vec<4, T> &glm_vec) {
        return Eigen::Matrix<T, 4, 1>(glm_vec[0], glm_vec[1], glm_vec[2], glm_vec[3]);
    }

    template<typename T>
    Eigen::Matrix<T, 2, 2> GlmToEigen(const glm::mat<2, 2, T> &glm_mat) {
        return Eigen::Matrix<T, 2, 2>(glm_mat[0][0], glm_mat[0][1],
                                      glm_mat[1][0], glm_mat[1][1]);
    }

    template<typename T>
    Eigen::Matrix<T, 3, 3> GlmToEigen(const glm::mat<3, 3, T> &glm_mat) {
        return Eigen::Matrix<T, 3, 3>(glm_mat[0][0], glm_mat[0][1], glm_mat[0][2],
                                      glm_mat[1][0], glm_mat[1][1], glm_mat[1][2],
                                      glm_mat[2][0], glm_mat[2][1], glm_mat[2][2]);
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> GlmToEigen(const glm::mat<4, 4, T> &glm_mat) {
        return Eigen::Matrix<T, 4, 4>(glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3],
                                      glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3],
                                      glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3],
                                      glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3]);
    }
}
#endif //ENGINE23_GLMTOEIGEN_H
