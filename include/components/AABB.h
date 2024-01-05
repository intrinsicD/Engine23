//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_AABB_H
#define ENGINE23_AABB_H

#include "glm/glm.hpp"
#include "components/ComponentGui.h"

namespace Bcg{
    struct AABB {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

        void grow(const glm::vec3 &point);

        void merge(const AABB &aabb);

        glm::vec3 get_center() const;

        glm::vec3 get_extent() const;

        void set(const glm::vec3 &center, const glm::vec3 &extent);
    };

    template<>
    struct ComponentGui<AABB> {
        static void Show(entt::entity entity_id);
    };
}

#endif //ENGINE23_AABB_H
