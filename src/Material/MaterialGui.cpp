//
// Created by alex on 14.06.24.
//

#include "MaterialGui.h"
#include "AttributeGui.h"
#include "Components.h"
#include "imgui.h"

namespace Bcg{
    void ComponentGui<Material>::Show(Material &material) {
        ImGui::Text("Name: %s", material.name.c_str());
        ImGui::Text("Program Index: %u", material.program_idx);
        if(ImGui::CollapsingHeader("Attributes")){
            Components<Attribute> attributes;
            for (auto &attribute_id: material.attributes) {
                auto &attribute = attributes.get_instance(attribute_id);
                if(ImGui::TreeNode(attribute.shader_attribute_name.c_str())){
                    ComponentGui<Attribute>::Show(attribute);
                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::CollapsingHeader("Uniforms")) {
            for (const auto &uniform: material.uniforms) {
                ImGui::Text("%u", uniform);
            }
        }
        if (ImGui::CollapsingHeader("Textures")) {
            for (const auto &texture: material.textures) {
                ImGui::Text("%u", texture);
            }
        }
    }
}