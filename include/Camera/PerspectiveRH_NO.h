//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_PERSPECTIVERH_NO_H
#define ENGINE23_PERSPECTIVERH_NO_H

#include "Eigen/Core"
#include <cassert>

namespace Bcg {
    template<typename T>
    Eigen::Matrix<T, 4, 4> PerspectiveRH_NO(T fovy, T aspect, T zNear, T zFar) {
        assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

        T const tanHalfFovy = tan(fovy / static_cast<T>(2));

        Eigen::Matrix<T, 4, 4> Result = Eigen::Matrix<T, 4, 4>::Identity();
        //TODO::Maybe switch rows and cols, because of different storage order in glm!?
        Result(0, 0) = static_cast<T>(1) / (aspect * tanHalfFovy);
        Result(1, 1) = static_cast<T>(1) / (tanHalfFovy);
        Result(2, 2) = -(zFar + zNear) / (zFar - zNear);
        Result(3, 2) = -static_cast<T>(1);
        Result(2, 3) = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
        return Result;
    }
}

#endif //ENGINE23_PERSPECTIVERH_NO_H
