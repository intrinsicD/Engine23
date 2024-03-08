//
// Created by alex on 05.01.24.
//

#include "Transform/SystemTransform.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "Transform.h"
#include "TransformGui.h"
#include "components/Picker.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {
        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemTransformInternal::on_render_gui>();
                return;
            }

            if (ImGui::Begin("Transform", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Transform>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Transform", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemTransformInternal::on_render_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemTransformInternal::on_render_gui_menu>();
            Log::Info(SystemTransform::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemTransformInternal::on_render_gui_menu>();
            Log::Info(SystemTransform::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTransform::name() {
        return "SystemTransform";
    }

    void SystemTransform::set_identity(entt::entity entity) {
        auto &transform = Engine::State().get<Transform>(entity);
        transform.model.setIdentity();
    }

    void SystemTransform::pre_init() {

    }

    void SystemTransform::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemTransformInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemTransform::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemTransformInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}