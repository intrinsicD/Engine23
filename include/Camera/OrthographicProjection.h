//
// Created by alex on 11.06.24.
//

#ifndef ENGINE23_ORTHOGRAPHICPROJECTION_H
#define ENGINE23_ORTHOGRAPHICPROJECTION_H

#include "OrthoRH_NO.h"

namespace Bcg{
    template<typename T>
    struct OrthographicProjection {
        T left;
        T right;
        T bottom;
        T top;
        T near;
        T far;
        bool dirty = false;

        Eigen::Matrix<T, 4, 4> get_matrix() const {
            return OrthoRH_NO(left, right, bottom, top, near, far);
        }
    };
}

#endif //ENGINE23_ORTHOGRAPHICPROJECTION_H
