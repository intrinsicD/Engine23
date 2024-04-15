//
// Created by alex on 09.04.24.
//

#ifndef ENGINE23_FRUSTUM_H
#define ENGINE23_FRUSTUM_H

#include "Camera.h"

namespace Bcg{
    class Frustum {
    public:
        Frustum() = default;

        Frustum(const Camera &camera);
    };
}

#endif //ENGINE23_FRUSTUM_H
