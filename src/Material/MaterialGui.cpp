//
// Created by alex on 14.06.24.
//

#include "MaterialGui.h"
#include "AttributeGui.h"
#include "UniformGui.h"
#include "TextureGui.h"
#include "Components.h"
#include "imgui.h"

namespace Bcg{

    void ComponentGui<Material>::Show(unsigned int material_id){
        Components<Material> materials;
        ComponentGui<Material>::Show(materials.get_instance(material_id));
    }

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
            Components<Uniform> uniforms;
            for (const auto &uniform_id: material.uniforms) {
                auto &uniform = uniforms.get_instance(uniform_id);
                if(ImGui::TreeNode(uniform.name.c_str())){
                    ComponentGui<Uniform>::Show(uniform);
                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::CollapsingHeader("Textures")) {
            Components<Texture> textures;
            for (const auto &texture_id: material.textures) {
                auto &texture = textures.get_instance(texture_id);
                if(ImGui::TreeNode(texture.name.c_str())){
                    ComponentGui<Texture>::Show(texture);
                    ImGui::TreePop();
                }
            }
        }
    }
}