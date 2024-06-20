//
// Created by alex on 02.01.24.
//

#ifndef ENGINE23_GLMTOEIGEN_H
#define ENGINE23_GLMTOEIGEN_H

#include "Eigen/Core"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Bcg {
    // General template function for mapping glm::vec to Eigen::Matrix
    template<typename T, int Size>
    Eigen::Map<Eigen::Vector<T, Size>> Map(glm::vec<Size, T> &glm) {
        return Eigen::Map<Eigen::Vector<T, Size>>(glm::value_ptr(glm));
    }

    template<typename T, int Rows, int Cols>
    Eigen::Map<Eigen::Matrix<T, Rows, Cols>> Map(glm::mat<Cols, Rows, T> &glm) {
        return Eigen::Map<Eigen::Matrix<T, Rows, Cols>>(glm::value_ptr(glm));
    }

    template<typename T, int Rows, int Cols>
    Eigen::Map<Eigen::Matrix<T, Rows, Cols>> Map(std::vector<glm::vec<Cols, T>> &data) {
        return Eigen::Map<Eigen::Matrix<T, Rows, Cols>>(reinterpret_cast<T *>(data.data()), Rows, Cols);
    }
}
#endif //ENGINE23_GLMTOEIGEN_H
