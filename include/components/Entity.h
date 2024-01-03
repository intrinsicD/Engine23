//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_ENTITY_H
#define ENGINE23_ENTITY_H

#include "entt/fwd.hpp"

namespace Bcg {
    class Entity {
    public:
        Entity(entt::registry *registry, entt::entity id) : registry(registry), id(id) {}

        virtual ~Entity() = default;

        entt::registry *registry;
        entt::entity id;
    };
}
#endif //ENGINE23_ENTITY_H
