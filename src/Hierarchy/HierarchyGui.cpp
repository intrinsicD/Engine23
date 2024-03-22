//
// Created by alex on 26.02.24.
//

#include "HierarchyGui.h"
#include "Hierarchy.h"
#include "Engine.h"
#include "imgui.h"
#include "Picker.h"

namespace Bcg {
    void gui_child(std::string name, entt::entity child_id) {
        if (child_id != entt::null) {
            ImGui::Text("%s: %u", name.c_str(), static_cast<unsigned int>(child_id));
            ImGui::SameLine();
            if (ImGui::Button(("Select##" + name + std::to_string(static_cast<unsigned int>(child_id))).c_str())) {
                auto &picker = Engine::Context().get<Picker>();
                picker.id.entity = child_id;
            }
        }
    }

    void ComponentGui<Hierarchy>::Show(entt::entity entity_id) {
        if (entity_id == entt::null ||
            !Engine::State().valid(entity_id)) {
            return;
        }

        if (!Engine::State().all_of<Hierarchy>(entity_id)) {
            if (ImGui::Button("Add Hierarchy Component")) {
                Engine::State().emplace<Hierarchy>(entity_id);
            }
        } else {
            auto &hierarchy = Engine::State().get<Hierarchy>(entity_id);
            if (ImGui::Button("Add Destroy Component")) {
                Engine::State().remove<Hierarchy>(entity_id);
            }
            ComponentGui<Hierarchy>::Show(hierarchy);
        }
    }

    void ComponentGui<Hierarchy>::Show(Hierarchy &hierarchy) {
        ImGui::Text("Num Children: %zu", hierarchy.children);
        gui_child("first", hierarchy.first);
        gui_child("prev", hierarchy.prev);
        gui_child("next", hierarchy.next);
        gui_child("parent", hierarchy.parent);
    }
}