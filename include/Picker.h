//
// Created by alex on 04.01.24.
//

#ifndef ENGINE23_PICKER_H
#define ENGINE23_PICKER_H

#include "EngineFwd.h"
#include "entt/entity/entity.hpp"
#include "glm/glm.hpp"
#include <set>

namespace Bcg {
    struct Picker {
        struct ID {
            entt::entity entity = entt::null;
            size_t vertex{};
            size_t edge{};
            size_t face{};

            bool check_entity() const { return entity != entt::null; }
        } id;

        struct Point {
            glm::vec3 object{};
            glm::vec3 world{};
            glm::vec3 view{};
        } point;

        bool is_backgound = false;

        float point_selection_radius = 0.00001;
        std::set<entt::entity> selected_entities{};

        enum class SelectionMode {
            entities,
            points,
            vertices,
            edges,
            faces
        } mode = SelectionMode::entities;
    };
}

#endif //ENGINE23_PICKER_H
