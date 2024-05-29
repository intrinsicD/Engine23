//
// Created by alex on 29.05.24.
//

#ifndef ENGINE23_PROJECTIONMATRIX_H
#define ENGINE23_PROJECTIONMATRIX_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T>
    class ProjectionMatrix {
    public:
        ProjectionMatrix();

        virtual ~ProjectionMatrix() = default;

        const Eigen::Matrix<T, 4, 4> &get_matrix() const {
            return m_matrix;
        }

        bool is_orthographic() const { return m_is_orthographic; }

    protected:
        Eigen::Matrix<T, 4, 4> m_matrix;
        bool m_is_orthographic;
    };


}

#endif //ENGINE23_PROJECTIONMATRIX_H
