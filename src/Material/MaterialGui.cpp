//
// Created by alex on 14.06.24.
//

#include "MaterialGui.h"
#include "imgui.h"

namespace Bcg{
    void ComponentGui<Material>::Show(Material &material) {
        ImGui::Text("Name: %s", material.name.c_str());
        ImGui::Text("Program Index: %u", material.program_idx);
        ImGui::Text("Attributes:");
        for (const auto &attribute: material.attributes) {
            ImGui::Text("%u", attribute);
        }
        ImGui::Text("Uniforms:");
        for (const auto &uniform: material.uniforms) {
            ImGui::Text("%u", uniform);
        }
        ImGui::Text("Textures:");
        for (const auto &texture: material.textures) {
            ImGui::Text("%u", texture);
        }
    }
}