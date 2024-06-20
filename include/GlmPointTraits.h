//
// Created by alex on 20.06.24.
//

#ifndef ENGINE23_GLMPOINTTRAITS_H
#define ENGINE23_GLMPOINTTRAITS_H

#include "PointTraits.h"
#include "glm/glm.hpp"

namespace Bcg::traits {

    // Generic scalar_type specialization for glm::tvec3
    template<typename T, glm::precision P>
    struct scalar_type<glm::tvec3<T, P>> {
        using type = T;
    };

    // Generic access specialization for glm::tvec3
    template<typename T, glm::precision P>
    struct access<glm::tvec3<T, P>, 0> {
        static auto get(const glm::tvec3<T, P> &p) {
            return p.x;
        }
    };

    template<typename T, glm::precision P>
    struct access<glm::tvec3<T, P>, 1> {
        static auto get(const glm::tvec3<T, P> &p) {
            return p.y;
        }
    };

    template<typename T, glm::precision P>
    struct access<glm::tvec3<T, P>, 2> {
        static auto get(const glm::tvec3<T, P> &p) {
            return p.z;
        }
    };
}

#endif //ENGINE23_GLMPOINTTRAITS_H
