//
// Created by alex on 22.02.24.
//

#include "MeshGui.h"
#include "Engine.h"
#include "imgui.h"

namespace Bcg{
    void ComponentGui<Mesh>::Show(entt::entity entity_id) {
        if(entity_id != entt::null && Engine::State().all_of<Mesh>(entity_id)) {
            Show(Engine::State().get<Mesh>(entity_id));
        }
    }

    void ComponentGui<Mesh>::Show(Mesh &mesh) {

    }
}