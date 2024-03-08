//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_ROTATION3D6DOF_H
#define ENGINE23_ROTATION3D6DOF_H

#include "CrossProductMatrix.h"
#include "SafeNormalization.h"

namespace Bcg {
    template<typename T>
    class Rotation3D6dof {
    public:
        Eigen::Matrix<T, 3, 2> m_params;

        Eigen::Matrix<T, 3, 3> matrix() const {
            Eigen::Matrix<T, 3, 3> rot;
            rot.col(0) = SafeNormalize(m_params.col(0), m_params.col(0).norm());
            rot.col(1) = m_params.col(1) - (rot.col(0).transpose() * m_params.col(1)) * rot.col(0);
            rot.col(1) = SafeNormalize(rot.col(1), rot.col(1).norm());
            rot.col(2) = rot.col(0).cross(rot.col(1));
            return rot;
        }

        Rotation3D6dof<T> from_matrix(const Eigen::Matrix<T, 3, 3> &rot) {
            rot = rot.leftCols(2);
            return *this;
        }
    };
}
#endif //ENGINE23_ROTATION3D6DOF_H
