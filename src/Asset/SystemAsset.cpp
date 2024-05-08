//
// Created by alex on 03.05.24.
//

#include "SystemAsset.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"
#include "Events.h"
#include "AssetGui.h"
#include "Entity.h"
#include "imgui.h"
#include "Picker.h"
#include "ImGuiUtils.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAssetInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAssetInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {
                if(ImGui::BeginMenu(SystemAsset::component_name().c_str())){
                    if (ImGui::MenuItem("Instance", nullptr, &show_gui_instance)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_instance>();
                    }
                    if (ImGui::MenuItem("Components", nullptr, &show_gui_components)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_components>();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui_instance(const Events::Update<Gui> &event) {
            if (!show_gui_instance) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemAsset::component_name().c_str(), &show_gui_instance)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Asset>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("MeshComponents", &show_gui_components)) {
                Components<Asset> components(SystemAsset::component_name());
                ImGuiUtils::Show(components);
            }
            ImGui::End();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemAsset::name() {
        return "System" + component_name();
    }

    std::string SystemAsset::component_name() {
        return "Asset";
    }

    void SystemAsset::pre_init() {

    }

    void SystemAsset::init() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemAssetInternal::on_update_gui_menu>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemAsset::remove() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemAssetInternal::on_update_gui_menu>();
        Log::Info("Removed", name()).enqueue();
    }
}