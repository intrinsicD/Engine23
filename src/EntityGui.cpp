//
// Created by alex on 10.06.24.
//

#include "imgui.h"
#include "Engine.h"
#include "EntityGui.h"
#include "AssetGui.h"
#include "AABBGui.h"
#include "TransformGui.h"

namespace Bcg{
    void ComponentGui<Entity>::Show(entt::entity entity_id){
        if (ImGui::BeginTabBar("Entity")) {
            if (ImGui::BeginTabItem("Info")) {
                ImGui::Text("ID: %zu", static_cast<size_t>(entity_id));
                if(Engine::State().all_of<std::string>(entity_id)){
                    ImGui::Text("Name: %s", Engine::State().get<std::string>(entity_id).c_str());
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Components")) {
                if(Engine::State().all_of<Asset>(entity_id)){
                    if(ImGui::CollapsingHeader("Asset")){
                        ComponentGui<Asset>::Show(entity_id);
                    }
                }
                if(Engine::State().all_of<AABB3>(entity_id)){
                    if(ImGui::CollapsingHeader("AABB")){
                        ComponentGui<AABB3>::Show(entity_id);
                    }
                }
                if(Engine::State().all_of<Transform<float>>(entity_id)){
                    if(ImGui::CollapsingHeader("Transform")){
                        ComponentGui<Transform<float>>::Show(entity_id);
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hierarchy")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Transform")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Geometry")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Topology")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Material")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Texture")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Animation")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Light")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Render")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Simulation")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Script")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Audio")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Physics")) {
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Misc")) {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
}