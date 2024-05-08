//
// Created by alex on 22.02.24.
//

#include "MeshGui.h"
#include "Engine.h"
#include "imgui.h"
#include "SystemMesh.h"
#include "Components.h"

namespace Bcg {
    void ComponentGui<Mesh>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<Component<Mesh>>(entity_id)) {
            ComponentGui<Mesh>::Show(Engine::State().get<Component<Mesh>>(entity_id));
        }
    }

    void ComponentGui<Mesh>::Show(Component<Mesh> &component) {
        Components<Mesh> meshes(SystemMesh::component_name());
        return Show(meshes.get_instance(component));
    }

    void ComponentGui<Mesh>::Show(Mesh &mesh) {
        ImGui::Text("TODO implement Mesh Gui");
    }
}