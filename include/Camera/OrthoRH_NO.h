//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_ORTHORH_NO_H
#define ENGINE23_ORTHORH_NO_H

#include "Eigen/Core"

namespace Bcg{
    template<typename T>
    Eigen::Matrix<T, 4, 4> OrthoRH_NO(T left, T right, T bottom, T top, T zNear, T zFar)
{
    Eigen::Matrix<T, 4, 4> Result = Eigen::Matrix<T, 4, 4>::Identity();
    //TODO::Maybe switch rows and cols, because of different storage order in glm!?
    Result(0, 0) = static_cast<T>(2) / (right - left);
    Result(1, 1) = static_cast<T>(2) / (top - bottom);
    Result(2, 2) = - static_cast<T>(2) / (zFar - zNear);
    Result(0, 3) = - (right + left) / (right - left);
    Result(1, 3) = - (top + bottom) / (top - bottom);
    Result(2, 3) = - (zFar + zNear) / (zFar - zNear);
    return Result;
}
}

#endif //ENGINE23_ORTHORH_NO_H
