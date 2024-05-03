//
// Created by alex on 22.02.24.
//

#include "MeshGui.h"
#include "Engine.h"
#include "imgui.h"
#include "ResourceContainer.h"
#include "Component.h"

namespace Bcg {
    void ComponentGui<Mesh>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<Component<Mesh>>(entity_id)) {
            auto &instances = Engine::Context().get<ResourceContainer<Mesh>>();
            auto mesh_id = Engine::State().get<Component<Mesh>>(entity_id).index;
            auto &mesh = instances.pool[mesh_id];
            Show(mesh);
        }
    }

    void ComponentGui<Mesh>::Show(Mesh &mesh) {
        ImGui::Text("TODO implement Mesh Gui");
    }
}