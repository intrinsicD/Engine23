//
// Created by alex on 02.01.24.
//

#ifndef ENGINE23_EIGENTOGLM_H
#define ENGINE23_EIGENTOGLM_H

#include "Eigen/Core"
#include "glm/glm.hpp"

namespace Bcg{
    template<typename T>
    glm::vec<2, T> EigenToGlm(const Eigen::Matrix<T, 2, 1> &eigen_vec) {
        return glm::vec<2, T>(eigen_vec[0], eigen_vec[1]);
    }

    template<typename T>
    glm::vec<3, T> EigenToGlm(const Eigen::Matrix<T, 3, 1> &eigen_vec) {
        return glm::vec<3, T>(eigen_vec[0], eigen_vec[1], eigen_vec[2]);
    }

    template<typename T>
    glm::vec<4, T> EigenToGlm(const Eigen::Matrix<T, 4, 1> &eigen_vec) {
        return glm::vec<4, T>(eigen_vec[0], eigen_vec[1], eigen_vec[2], eigen_vec[3]);
    }

    template<typename T>
    glm::mat<2, 2, T> EigenToGlm(const Eigen::Matrix<T, 2, 2> &eigen_mat) {
        return glm::mat<2, 2, T>(eigen_mat(0, 0), eigen_mat(0, 1),
                                 eigen_mat(1, 0), eigen_mat(1, 1));
    }

    template<typename T>
    glm::mat<3, 3, T> EigenToGlm(const Eigen::Matrix<T, 3, 3> &eigen_mat) {
        return glm::mat<3, 3, T>(eigen_mat(0, 0), eigen_mat(0, 1), eigen_mat(0, 2),
                                 eigen_mat(1, 0), eigen_mat(1, 1), eigen_mat(1, 2),
                                 eigen_mat(2, 0), eigen_mat(2, 1), eigen_mat(2, 2));
    }

    template<typename T>
    glm::mat<4, 4, T> EigenToGlm(const Eigen::Matrix<T, 4, 4> &eigen_mat) {
        return glm::mat<4, 4, T>(eigen_mat(0, 0), eigen_mat(0, 1), eigen_mat(0, 2), eigen_mat(0, 3),
                                 eigen_mat(1, 0), eigen_mat(1, 1), eigen_mat(1, 2), eigen_mat(1, 3),
                                 eigen_mat(2, 0), eigen_mat(2, 1), eigen_mat(2, 2), eigen_mat(2, 3),
                                 eigen_mat(3, 0), eigen_mat(3, 1), eigen_mat(3, 2), eigen_mat(3, 3));
    }
}

#endif //ENGINE23_EIGENTOGLM_H
