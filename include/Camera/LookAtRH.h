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

        Eigen::Matrix<T, 4, 4> view = Eigen::Matrix<T, 4, 4>::Identity();
        //hide TODO::Maybe switch rows and cols, because of different storage order in glm!?
        view(0, 0) = s[0];
        view(0, 1) = s[1];
        view(0, 2) = s[2];
        view(1, 0) = u[0];
        view(1, 1) = u[1];
        view(1, 2) = u[2];
        view(2, 0) = -f[0];
        view(2, 1) = -f[1];
        view(2, 2) = -f[2];
        view(0, 3) = -s.transpose() * eye;
        view(1, 3) = -u.transpose() * eye;
        view(2, 3) = f.transpose() * eye;
        return view;
    }
}

#endif //ENGINE23_LOOKATRH_H
