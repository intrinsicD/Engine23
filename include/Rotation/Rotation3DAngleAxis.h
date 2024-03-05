//
// Created by alex on 05.03.24.
//

#ifndef ENGINE23_ROTATION3DANGLEAXIS_H
#define ENGINE23_ROTATION3DANGLEAXIS_H

#include "RotationSkewSymmetricHelper.h"

namespace Bcg {
    template<typename T>
    class Rotation3DAngleAxis {
        Eigen::Vector<T, 3> m_angle_axis;

        Eigen::Matrix<T, 3, 3> matrix() const {
            //Rodrigues formula
            T angle = m_angle_axis.norm();
            Eigen::Matrix<T, 3, 3> K = skew_symmetric_helper(m_angle_axis /(angle + 1e-8));
            Eigen::Matrix<T, 3, 3> I = Eigen::Matrix<T, 3, 3>::Identity();
            return I + K * std::sin(angle) + K * K * (1 - std::cos(angle));
        }

        Rotation3DAngleAxis<T> from_matrix(const Eigen::Matrix<T, 3, 3> &rot) {
            T trace = rot.trace();
            T angle = std::acos((trace - 1) / 2);
            Eigen::Vector<T, 3> axis(rot(2, 1) - rot(1, 2), rot(0, 2) - rot(2, 0), rot(1, 0) - rot(0, 1));
            m_angle_axis = axis.normalized() * angle;
            return *this;
        }
    };
}

#endif //ENGINE23_ROTATION3DANGLEAXIS_H
