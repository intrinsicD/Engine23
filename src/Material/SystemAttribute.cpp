//
// Created by alex on 14.06.24.
//

#include "SystemAttribute.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "TypeName.h"
#include "Attribute.h"
#include "AttributeGui.h"
#include "Components.h"
#include "imgui.h"
#include "ImGuiUtils.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAttributeInternal {
        static bool show_gui_components = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAttributeInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu(TypeName<Attribute>::name().c_str())) {
                    if (ImGui::MenuItem("Components", nullptr, &show_gui_components)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_components>();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("AttributeComponents", &show_gui_components)) {
                Components<Attribute> attributes;
                ImGuiUtils::Show(attributes);
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
    std::string SystemAttribute::name() {
        return "System" + component_name();
    }

    std::string SystemAttribute::component_name() {
        return TypeName<Attribute>::name();
    }

    void SystemAttribute::pre_init() {

    }

    void SystemAttribute::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemAttributeInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemAttributeInternal::on_shutdown_engine>();
        Log::SystemInit(name()).enqueue();
    }

    void SystemAttribute::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemAttributeInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemAttributeInternal::on_shutdown_engine>();
        Log::SystemRemove(name()).enqueue();
    }
}