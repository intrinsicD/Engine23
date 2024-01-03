//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_GLMUTILS_H
#define ENGINE23_GLMUTILS_H

#include <vector>
#include "glm/glm.hpp"

namespace Bcg{
    inline auto dims(const glm::vec2 &v) -> size_t {
        return 2;
    }

    inline auto dims(const glm::uvec2 &v) -> size_t {
        return 2;
    }

    inline auto dims(const glm::vec3 &v) -> size_t {
        return 3;
    }

    inline auto dims(const glm::uvec3 &v) -> size_t {
        return 3;
    }

    inline auto dims(const glm::vec4 &v) -> size_t {
        return 4;
    }

    template<typename T>
    inline auto dims(const std::vector<T> &v) -> size_t {
        return dims(v[0]);
    }
}

#endif //ENGINE23_GLMUTILS_H
