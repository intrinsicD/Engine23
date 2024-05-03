//
// Created by alex on 26.02.24.
//

#include "AssetGui.h"
#include "Asset.h"
#include "Engine.h"
#include "ResourceContainer.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Bcg {
    static bool edit_asset = false;

    void ComponentGui<Asset>::Show(entt::entity entity_id) {
        if (entity_id == entt::null ||
            !Engine::State().valid(entity_id) ||
            !Engine::State().all_of<Component<Asset>>(entity_id)) {
            return;
        } else {
            auto &component = Engine::State().get<Component<Asset>>(entity_id);
            Show(component);
        }
    }

    void ComponentGui<Asset>::Show(Component<Asset> component) {
        auto &assets = Engine::Context().get<ResourceContainer<Asset>>();
        return Show(assets.pool[component.index]);
    }

    void ComponentGui<Asset>::Show(Asset &asset) {
        ImGui::Checkbox("Edit##Asset", &edit_asset);
        if (edit_asset) {
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

