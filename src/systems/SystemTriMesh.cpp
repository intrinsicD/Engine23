//
// Created by alex on 10.01.24.
//

#include "systems/SystemTriMesh.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "Picker.h"
#include "TriMesh.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTriMeshInternal {
        static bool show_gui = false;

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_render_gui(const Events::Render<Gui> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTriMeshInternal {
        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("TriMesh", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&on_render_gui>();
                return;
            }

            if (ImGui::Begin("TriMesh", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                if(picker.id.entity != entt::null && Engine::State().all_of<TriMesh>(picker.id.entity)){
                    auto &mesh = Engine::State().get<TriMesh>(picker.id.entity);
                    ComponentGui<TriMesh>::Show("", mesh);
                }
            }
            ImGui::End();

        }

        void on_startup_engine(const Events::Startup<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&on_render_gui_menu>();
            Log::Info(SystemTriMesh::name() , "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&on_render_gui_menu>();
            Log::Info(SystemTriMesh::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTriMesh::name() {
        return "TriMesh";
    }

    void SystemTriMesh::pre_init() {

    }

    void SystemTriMesh::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemTriMeshInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemTriMeshInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemTriMesh::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemTriMeshInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemTriMeshInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}