//
// Created by alex on 03.05.24.
//

#include "SystemAsset.h"
#include "Engine.h"
#include "Commands.h"
#include "Component.h"
#include "Events.h"
#include "Asset.h"
#include "Entity.h"
#include "imgui.h"
#include "Picker.h"
#include "ResourceContainer.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAssetInternal {
        static bool show_gui = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);

    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAssetInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Asset", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().connect<&SystemAssetInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event){
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().disconnect<&SystemAssetInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Asset", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Asset>::Show(picker.id.entity);
            }
            ImGui::End();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    using ResourceContainerAsset = ResourceContainer<Asset>;

    std::string SystemAsset::name() {
        return "System" + component_name();
    }

    std::string SystemAsset::component_name() {
        return "Asset";
    }

    unsigned int SystemAsset::create_instance() {
        auto &instances = Engine::Context().get<ResourceContainerAsset>();
        if (!instances.free_list.empty()) {
            unsigned int instance_id = instances.free_list.back();
            instances.free_list.pop_back();
            instances.pool[instance_id] = Asset();
            Log::Info("Reuse " + component_name() + " instance with instance_id: " +
                      std::to_string(instance_id)).enqueue();
            return instance_id;
        } else {
            unsigned int instance_id = instances.get_size();
            instances.push_back();
            Log::Info("Created " + component_name() + " instance with instance_id: " +
                      std::to_string(instance_id)).enqueue();
            return instance_id;
        }
    }

    void SystemAsset::destroy_instance(unsigned int instance_id) {
        auto &instances = Engine::Context().get<ResourceContainerAsset>();
        instances.free_list.push_back(instance_id);
        Log::Info(
                "Destroy " + component_name() + " instance with instance_id: " + std::to_string(instance_id)).enqueue();
    }

    void SystemAsset::add_to_entity(entt::entity entity_id, unsigned int instance_id) {
        Engine::State().emplace_or_replace<Component<Asset>>(entity_id, instance_id);
        Log::Info(
                "Add " + component_name() + " with instance_id: " + std::to_string(instance_id) + " to entity_id: " +
                AsString(entity_id)).enqueue();
    }

    void SystemAsset::remove_from_entity(entt::entity entity_id) {
        Engine::State().remove<Component<Asset>>(entity_id);
        Log::Info("Removed " + component_name() + " from entity_id: " + AsString(entity_id)).enqueue();
    }

    void SystemAsset::pre_init() {
        Engine::Context().emplace<ResourceContainerAsset>();
    }

    void SystemAsset::init() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemAssetInternal::on_update_gui_menu>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemAsset::remove() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemAssetInternal::on_update_gui_menu>();
        Engine::Context().erase<ResourceContainerAsset>();
        Log::Info("Removed", name()).enqueue();
    }
}