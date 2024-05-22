//
// Created by alex on 04.01.24.
//

#ifndef ENGINE23_PICKER_H
#define ENGINE23_PICKER_H

#include "EngineFwd.h"
#include "entt/entity/entity.hpp"
#include "Eigen/Core"
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
            bool is_background = false;
            Eigen::Vector<float, 3> object{};
            Eigen::Vector<float, 3> world{};
            Eigen::Vector<float, 3> view{};
            Eigen::Vector<float, 2> win_coords{};
            Eigen::Vector<float, 2> norm_dev_coords{};
        } point;

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
