//
// Created by alex on 05.03.24.
//

#ifndef ENGINE23_ROTATION3DCAYLEY_H
#define ENGINE23_ROTATION3DCAYLEY_H

#include "CrossProductMatrix.h"

namespace Bcg {
    template<typename T>
    class Rotation3DCayley {
    public:
        Eigen::Vector<T, 3> m_z;

        T get_angle() const {
            return 2 * std::atan(m_z.norm());
        }

        Eigen::Vector<T, 3> get_axis() const{
            return m_z.normalized();
        }

        Eigen::Vector<T, 3> angle_axis_from_cayley() const {
            T length = m_z.norm();
            T angle = 2 * std::atan(length);
            return angle * m_z / length;
        }

        Eigen::Matrix<T, 3, 3> matrix() const {
            Eigen::Matrix<T, 3, 3> K = CrossProductMatrix(m_z);
            Eigen::Matrix<T, 3, 3> Id = Eigen::Matrix<T, 3, 3>::Identity();
            return (Id - K) * (Id + K).inverse();
        }

        Rotation3DCayley<T> from_matrix(const Eigen::Matrix<T, 3, 3> &rot) {
            m_z << rot(1, 2) - rot(2, 1), rot(2, 0) - rot(0, 2), rot(0, 1) - rot(1, 0);
            m_z /= 2;
            return *this;
        }
    };
}

#endif //ENGINE23_ROTATION3DCAYLEY_H
