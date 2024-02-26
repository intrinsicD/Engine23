//
// Created by alex on 26.02.24.
//

#include "AssetGui.h"
#include "Asset.h"
#include "Engine.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Bcg {
    static bool edit_asset = false;

    void ComponentGui<Asset>::Show(entt::entity entity_id) {
        if (entity_id == entt::null ||
            !Engine::State().valid(entity_id) ||
            !Engine::State().all_of<Asset>(entity_id)){
            return;
        }else{
            Asset &asset = Engine::State().get<Asset>(entity_id);
            Show(asset);
        }
    }

    void ComponentGui<Asset>::Show(Asset &asset) {
        if (ImGui::Checkbox("Edit##Asset", &edit_asset)) {
            ImGui::InputText("Name##Asset", &asset.name);
            ImGui::Text("Filepath: %s", asset.filepath.c_str());
            ImGui::InputText("Type##Asset", &asset.name);
        } else {
            ImGui::Text("Name: %s", asset.name.c_str());
            ImGui::Text("Filepath: %s", asset.filepath.c_str());
            ImGui::Text("Type: %s", asset.type.c_str());
        }
    }
}

