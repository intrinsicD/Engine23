//
// Created by alex on 14.06.24.
//

#include "ActiveMaterialsGui.h"
#include "imgui.h"
#include "Engine.h"
#include "Components.h"
#include "MaterialGui.h"
#include "fmt/core.h"

namespace Bcg {
    void ComponentGui<ActiveMaterials>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<Component<ActiveMaterials>>(entity_id)) {
            ComponentGui<ActiveMaterials>::Show(Engine::State().get<Component<ActiveMaterials>>(entity_id));
        }
    }

    void ComponentGui<ActiveMaterials>::Show(Component<ActiveMaterials> &component) {
        Components<ActiveMaterials> active_materials;
        return Show(active_materials.get_instance(component));
    }

    void ComponentGui<ActiveMaterials>::Show(Bcg::ActiveMaterials &active_materials) {
        Components<Material> materials;
        if(ImGui::CollapsingHeader(fmt::format("Mesh Material Index: {}", active_materials.mesh_material_idx).c_str())){
            ComponentGui<Material>::Show(active_materials.mesh_material_idx);
        }
        if(ImGui::CollapsingHeader(fmt::format("Graph Material Index: {}", active_materials.graph_material_idx).c_str())){
            ComponentGui<Material>::Show(active_materials.graph_material_idx);
        }
        if(ImGui::CollapsingHeader(fmt::format("Point Cloud Material Index: {}", active_materials.point_cloud_material_idx).c_str())){
            ComponentGui<Material>::Show(active_materials.point_cloud_material_idx);
        }
        if (ImGui::CollapsingHeader("Vertex Vector Field Indices:")) {
            for (const auto &index: active_materials.vertex_vector_field_indices) {
                if(ImGui::TreeNode(fmt::format("{}", index).c_str())) {
                    ComponentGui<Material>::Show(index);
                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::CollapsingHeader("Edge Vector Field Indices:")) {
            for (const auto &index: active_materials.edge_vector_field_indices) {
                if(ImGui::TreeNode(fmt::format("{}", index).c_str())) {
                    ComponentGui<Material>::Show(index);
                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::CollapsingHeader("Face Vector Field Indices:")) {
            for (const auto &index: active_materials.face_vector_field_indices) {
                if(ImGui::TreeNode(fmt::format("{}", index).c_str())) {
                    ComponentGui<Material>::Show(index);
                    ImGui::TreePop();
                }
            }
        }
    }
}