//
// Created by alex on 11.06.24.
//

#ifndef ENGINE23_PERSPECTIVEPROJECTION_H
#define ENGINE23_PERSPECTIVEPROJECTION_H

#include "PerspectiveRH_NO.h"
#include "DegreesToRadians.h"

namespace Bcg{
    template<typename T>
    struct PerspectiveProjection {
        T fovy_degrees = 45.0;
        T aspect = 4.0 / 3.0;
        T near;
        T far;
        bool dirty = false;

        Eigen::Matrix<T, 4, 4> get_matrix() const {
            return PerspectiveRH_NO(DegreesToRadians(fovy_degrees), aspect, near, far);
        }
    };
}
#endif //ENGINE23_PERSPECTIVEPROJECTION_H
