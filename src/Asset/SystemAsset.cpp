//
// Created by alex on 03.05.24.
//

#include "SystemAsset.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"
#include "Events.h"
#include "Asset.h"
#include "Entity.h"
#include "imgui.h"
#include "Picker.h"


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
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemAssetInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event){
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemAssetInternal::on_update_gui>();
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