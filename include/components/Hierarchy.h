//
// Created by alex on 18.01.24.
//

#ifndef ENGINE23_HIERARCHY_H
#define ENGINE23_HIERARCHY_H

#include "entt/entt.hpp"

namespace Bcg{
    struct Hierarchy{
        std::size_t children{};
        entt::entity first{entt::null};
        entt::entity prev{entt::null};
        entt::entity next{entt::null};
        entt::entity parent{entt::null};
    };
}

#endif //ENGINE23_HIERARCHY_H
