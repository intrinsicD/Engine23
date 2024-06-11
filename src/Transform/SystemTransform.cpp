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
#include "ImGuiUtils.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

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

        void on_update_gui_instance(const Events::Update<Gui> &event) {
            if (!show_gui_instance) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemTransformInternal::on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemTransform::component_name().c_str(), &show_gui_instance, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Transform<float>>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("TransformComponents", &show_gui_components)) {
                Components<Transform<float>> transforms;
                ImGuiUtils::Show(transforms);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if(ImGui::BeginMenu(SystemTransform::component_name().c_str())){
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
    BCG_GENERATE_TYPE_STRING(Transform<float>)

    std::string SystemTransform::name() {
        return "System" + component_name();
    }

    std::string SystemTransform::component_name() {
        return TypeName<Transform<float>>::name;
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