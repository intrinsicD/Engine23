//
// Created by alex on 27.05.24.
//

#include "TextureGui.h"
#include "Engine.h"
#include "Components.h"
#include "SystemTexture.h"
#include "imgui.h"

namespace Bcg {
    void ComponentGui<Texture>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<Component<Texture>>(entity_id)) {
            return;
        }
        auto &component_texture = Engine::State().get<Component<Texture>>(entity_id);
        Show(component_texture);
    }

    void ComponentGui<Texture>::Show(Component<Texture> &component) {
        Components<Texture> textures;
        Show(textures.get_instance(component));
    }

    void ComponentGui<Texture>::Show(Texture &texture) {
        ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture.id)),
                     ImVec2(texture.width, texture.height));
    }
}