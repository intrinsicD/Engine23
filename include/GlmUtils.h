//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_GLMUTILS_H
#define ENGINE23_GLMUTILS_H

#include <vector>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "ImGuiUtils.h"

namespace Bcg {
    template<typename T>
    std::string ToString(const T &v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    // Function to convert various types to string
    template<>
    std::string ToString(const glm::vec2 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y;
        return ss.str();
    }

    template<>
    std::string ToString(const glm::uvec2 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y;
        return ss.str();
    }

    template<>
    std::string ToString(const glm::vec3 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y << " " << v.z;
        return ss.str();
    }

    template<>
    std::string ToString(const glm::uvec3 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y << " " << v.z;
        return ss.str();
    }

    template<>
    std::string ToString(const glm::vec4 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y << " " << v.z << " " << v.w;
        return ss.str();
    }

    template<>
    std::string ToString(const glm::uvec4 &v) {
        std::stringstream ss;
        ss << v.x << " " << v.y << " " << v.z << " " << v.w;
        return ss.str();
    }

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
