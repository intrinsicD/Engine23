//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_LOOKATRH_H
#define ENGINE23_LOOKATRH_H

#include "Eigen/Geometry"

namespace Bcg {
    template<typename T>
    Eigen::Matrix<T, 4, 4>
    LookAtRH(const Eigen::Vector<T, 3> &eye, const Eigen::Vector<T, 3> &center, const Eigen::Vector<T, 3> &up) {
        const Eigen::Vector<T, 3> f((center - eye).normalized());
        const Eigen::Vector<T, 3> s(f.cross(up).normalized());
        const Eigen::Vector<T, 3> u(s.cross(f));

        Eigen::Matrix<T, 4, 4> Result = Eigen::Matrix<T, 4, 4>::Identity();
        //hide TODO::Maybe switch rows and cols, because of different storage order in glm!?
        Result(0, 0) = s[0];
        Result(0, 1) = s[1];
        Result(0, 2) = s[2];
        Result(1, 0) = u[0];
        Result(1, 1) = u[1];
        Result(1, 2) = u[2];
        Result(2, 0) = -f[0];
        Result(2, 1) = -f[1];
        Result(2, 2) = -f[2];
        Result(0, 3) = -s.transpose() * eye;
        Result(1, 3) = -u.transpose() * eye;
        Result(2, 3) = f.transpose() * eye;
        return Result;
    }
}

#endif //ENGINE23_LOOKATRH_H
