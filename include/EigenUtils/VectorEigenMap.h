//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_VECTOREIGENMAP_H
#define ENGINE23_VECTOREIGENMAP_H

#include "Eigen/Core"
#include <vector>

namespace Bcg {

    template<typename T>
    Eigen::Map<Eigen::Vector<T, -1>> Map(std::vector<T> &points) {
        return Eigen::Map<Eigen::Vector<T, -1>>(points.data(), points.size(), 1);
    }

    template<typename T>
    Eigen::Map<const Eigen::Vector<T, -1>> MapConst(const std::vector<T> &points) {
        return Eigen::Map<const Eigen::Vector<T, -1>>(points.data(), points.size(), 1);
    }

};

#endif //ENGINE23_VECTOREIGENMAP_H
