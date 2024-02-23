//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_ENTITY_H
#define ENGINE23_ENTITY_H

#include "ComponentGui.h"
#include "Mesh.h"
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
        T &replace(T &&t) {
            return registry.replace<T>(id, std::forward<T>(t));
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

        [[nodiscard]] VertexContainer *vertices() const {
            if (Engine::State().all_of<Mesh>(id)) {
                return &Engine::State().get<Mesh>(id).vertices;
            } else if (Engine::State().all_of<Graph>(id)) {
                return &Engine::State().get<Graph>(id).vertices;
            } else if (Engine::State().all_of<PointCloud>(id)) {
                return &Engine::State().get<PointCloud>(id).vertices;
            } else {
                return nullptr;
            }
        }

        [[nodiscard]] HalfedgeContainer *halfedges() const {
            if (Engine::State().all_of<Mesh>(id)) {
                return &Engine::State().get<Mesh>(id).halfedges;
            } else if (Engine::State().all_of<Graph>(id)) {
                return &Engine::State().get<Graph>(id).halfedges;
            } else {
                return nullptr;
            }
        }

        [[nodiscard]] EdgeContainer *edges() const {
            if (Engine::State().all_of<Mesh>(id)) {
                return &Engine::State().get<Mesh>(id).edges;
            } else if (Engine::State().all_of<Graph>(id)) {
                return &Engine::State().get<Graph>(id).edges;
            } else {
                return nullptr;
            }
        }

        [[nodiscard]] FaceContainer *faces() const {
            if (Engine::State().all_of<Mesh>(id)) {
                return &Engine::State().get<Mesh>(id).faces;
            } else {
                return nullptr;
            }
        }

    };

    template<>
    struct ComponentGui<Entity> {
        static void Show(entt::entity entity_id);
    };
}
#endif //ENGINE23_ENTITY_H
