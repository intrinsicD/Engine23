//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_ENTITY_H
#define ENGINE23_ENTITY_H

#include "ComponentGui.h"
#include "entt/entity/registry.hpp"

namespace Bcg {
    class Entity {
    public:
        Entity(entt::registry &registry, entt::entity id) : registry(registry), id(id) {}

        virtual ~Entity() = default;

        entt::registry &registry;
        entt::entity id;

        bool is_valid() const {
            return id != entt::null && registry.valid(id);
        }

        template<typename T>
        bool all_of() {
            return registry.all_of<T>(id);
        }

        template<typename T>
        T &emplace(T &&t) {
            return registry.emplace<T>(id, std::forward<T>(t));
        }

        template<typename T>
        T &get() {
            return registry.get<T>(id);
        }

        template<typename T>
        T &get_or_emplace(T &&t) {
            return registry.get_or_emplace<T>(std::forward<T>(t));
        }

        template<typename T>
        T &remove() {
            return registry.remove<T>(id);
        }
    };

    template<>
    struct ComponentGui<Entity> {
        static void Show(entt::entity entity_id);
    };
}
#endif //ENGINE23_ENTITY_H
