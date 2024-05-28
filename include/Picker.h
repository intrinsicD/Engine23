//
// Created by alex on 04.01.24.
//

#ifndef ENGINE23_PICKER_H
#define ENGINE23_PICKER_H

#include "EngineFwd.h"
#include "ClickPoint.h"
#include "entt/entity/entity.hpp"
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

        Point point;

        float point_selection_radius = 0.00001;
        std::vector<entt::entity> selected_entities{};

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
