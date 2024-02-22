//
// Created by alex on 22.02.24.
//

#include "SystemMesh.h"
#include "Engine.h"
#include "Commands.h"
#include "Events.h"
#include "imgui.h"
#include "MeshGui.h"
#include "MeshIo.h"
#include "components/Input.h"
#include "components/Picker.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMeshInternal {
        static bool show_gui = false;

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_render_gui(const Events::Render<Gui> &event);

        void on_update_input_drop(const Events::Update<Input::Drop> &event);

        void on_load_mesh(const Events::Load<Mesh> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMeshInternal {
        void on_startup(const Events::Startup<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemMeshInternal::on_render_gui_menu>();
            Log::Info(SystemMesh::name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemMeshInternal::on_render_gui_menu>();
            Log::Info(SystemMesh::name() + ": Shutdown").enqueue();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Mesh", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_render_gui(const Events::Render<Gui> &event){
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&on_render_gui>();
                return;
            }

            if (ImGui::Begin("TriMesh", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                if(picker.id.entity != entt::null && Engine::State().all_of<Mesh>(picker.id.entity)){
                    auto &mesh = Engine::State().get<Mesh>(picker.id.entity);
                    ComponentGui<Mesh>::Show(mesh);
                }
            }
            ImGui::End();
        }

        void on_update_input_drop(const Events::Update<Input::Drop> &event){

        }

        void on_load_mesh(const Events::Load<Mesh> &event){
            SystemMesh::load(event.filepath, event.entity_id);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemMesh::name() {
        return "SystemMesh";
    }

    bool SystemMesh::load(const std::string &filepath, entt::entity entity){
        MeshIo reader(filepath);
        Mesh mesh;
        if(!reader.read(mesh)) {
            Log::Warn("Failed to load mesh from file: " + filepath).enqueue();
            return false;
        }else{
            Log::Info("Loaded mesh from file: " + filepath).enqueue();
        }

        Engine::State().emplace<Mesh>(entity, mesh);
        return true;
    }

    void SystemMesh::pre_init() {

    }

    void SystemMesh::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemMeshInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemMeshInternal::on_render_gui_menu>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemMeshInternal::on_shutdown>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemMesh::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemMeshInternal::on_startup>();

        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemMeshInternal::on_shutdown>();
        Log::Info(name() + ": Removed").enqueue();
    }
}