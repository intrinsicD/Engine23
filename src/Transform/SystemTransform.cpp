//
// Created by alex on 05.01.24.
//

#include "Transform/SystemTransform.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "Transform.h"
#include "TransformGui.h"
#include "Picker.h"
#include "Entity.h"
#include "imgui.h"
#include "Components.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemTransformInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Transform", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Transform<float>>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Transform", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemTransformInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemTransformInternal::on_update_gui_menu>();
            Log::Info(SystemTransform::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemTransformInternal::on_update_gui_menu>();
            Log::Info(SystemTransform::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTransform::name() {
        return "System" + component_name();
    }

    std::string SystemTransform::component_name() {
        return "Transform";
    }

    void SystemTransform::pre_init() {

    }

    void SystemTransform::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemTransformInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemTransform::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemTransformInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}