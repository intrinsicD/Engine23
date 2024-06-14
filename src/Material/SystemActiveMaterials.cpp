//
// Created by alex on 14.06.24.
//

#include "SystemActiveMaterials.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "TypeName.h"
#include "ActiveMaterials.h"
#include "Components.h"
#include "ActiveMaterialsGui.h"
#include "Picker.h"
#include "imgui.h"
#include "ImGuiUtils.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemActiveMaterialsInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemActiveMaterialsInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu(TypeName<ActiveMaterials>::name().c_str())) {
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

            if (ImGui::Begin(TypeName<ActiveMaterials>::name().c_str(), &show_gui_instance,
                             ImGuiChildFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                if (!Engine::State().all_of<Component<ActiveMaterials>>(picker.id.entity)) {
                    if (ImGui::Button("Add")) {
                        Components<ActiveMaterials> active_materials;
                        auto instance_idx = active_materials.create_instance();
                        Engine::State().emplace<Component<ActiveMaterials>>(picker.id.entity, instance_idx);
                    }
                }
                ComponentGui<ActiveMaterials>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("ActiveMaterialComponents", &show_gui_components)) {
                Components<ActiveMaterials> active_materials;
                ImGuiUtils::Show(active_materials);
            }
            ImGui::End();
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&on_update_gui_menu>();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&on_update_gui_menu>();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemActiveMaterials::name() {
        return "System" + component_name();
    }

    std::string SystemActiveMaterials::component_name() {
        return TypeName<ActiveMaterials>::name();
    }

    void SystemActiveMaterials::pre_init() {

    }

    void SystemActiveMaterials::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemActiveMaterialsInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemActiveMaterialsInternal::on_shutdown_engine>();
        Log::SystemInit(name()).enqueue();
    }

    void SystemActiveMaterials::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemActiveMaterialsInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemActiveMaterialsInternal::on_shutdown_engine>();
        Log::SystemRemove(name()).enqueue();
    }
}