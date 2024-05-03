//
// Created by alex on 03.05.24.
//

#include "Entity.h"
#include "Component.h"
#include "Engine.h"
#include "ResourceContainer.h"

namespace Bcg {
    Mesh &get_mesh(entt::entity entity_id) {
        auto mesh_id = Engine::State().get<Component<Mesh>>(entity_id).index;
        auto &instances = Engine::Context().get<ResourceContainer<Mesh>>();
        return instances.pool[mesh_id];
    }

    Graph &get_graph(entt::entity entity_id) {
        auto graph_id = Engine::State().get<Component<Graph>>(entity_id).index;
        auto &instances = Engine::Context().get<ResourceContainer<Graph>>();
        return instances.pool[graph_id];
    }

    PointCloud &get_point_cloud(entt::entity entity_id) {
        auto pc_id = Engine::State().get<Component<PointCloud>>(entity_id).index;
        auto &instances = Engine::Context().get<ResourceContainer<PointCloud>>();
        return instances.pool[pc_id];
    }

    [[nodiscard]] VertexContainer *Entity::vertices() const {
        if (Engine::State().all_of<Component<Mesh>>(id)) {
            return &get_mesh(id).vertices;
        } else if (Engine::State().all_of<Component<Graph>>(id)) {
            return &get_graph(id).vertices;
        } else if (Engine::State().all_of<Component<PointCloud>>(id)) {
            return &get_point_cloud(id).vertices;
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] HalfedgeContainer *Entity::halfedges() const {
        if (Engine::State().all_of<Component<Mesh>>(id)) {
            return &get_mesh(id).halfedges;
        } else if (Engine::State().all_of<Component<Graph>>(id)) {
            return &get_graph(id).halfedges;
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] EdgeContainer *Entity::edges() const {
        if (Engine::State().all_of<Component<Mesh>>(id)) {
            return &get_mesh(id).edges;
        } else if (Engine::State().all_of<Component<Graph>>(id)) {
            return &get_graph(id).edges;
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] FaceContainer *Entity::faces() const {
        if (Engine::State().all_of<Component<Mesh>>(id)) {
            return &get_mesh(id).faces;
        } else {
            return nullptr;
        }
    }
}