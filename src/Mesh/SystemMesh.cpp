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
#include "fmt/core.h"
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

        void on_construct(entt::registry &, entt::entity);

        void on_update(entt::registry &, entt::entity);

        void on_destroy(entt::registry &, entt::entity);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMeshInternal {
        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemMeshInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&SystemMeshInternal::on_update_input_drop>();
            Engine::Instance()->dispatcher.sink<Events::Load<Mesh>>().connect<&SystemMeshInternal::on_load_mesh>();
            Engine::State().on_construct<Mesh>().connect<&SystemMeshInternal::on_construct>();
            Engine::State().on_update<Mesh>().connect<&SystemMeshInternal::on_update>();
            Engine::State().on_destroy<Mesh>().connect<&SystemMeshInternal::on_destroy>();
            Log::Info(SystemMesh::name() , "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemMeshInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&SystemMeshInternal::on_update_input_drop>();
            Engine::Instance()->dispatcher.sink<Events::Load<Mesh>>().disconnect<&SystemMeshInternal::on_load_mesh>();
            Engine::State().on_construct<Mesh>().disconnect<&SystemMeshInternal::on_construct>();
            Engine::State().on_update<Mesh>().disconnect<&SystemMeshInternal::on_update>();
            Engine::State().on_destroy<Mesh>().disconnect<&SystemMeshInternal::on_destroy>();
            Log::Info(SystemMesh::name() , "Shutdown").enqueue();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Mesh", nullptr, &show_gui)) {
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
                if (picker.id.entity != entt::null && Engine::State().all_of<Mesh>(picker.id.entity)) {
                    auto &mesh = Engine::State().get<Mesh>(picker.id.entity);
                    ComponentGui<Mesh>::Show(mesh);
                }
            }
            ImGui::End();
        }

        void on_update_input_drop(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            for (const auto &path: input.drop.paths) {
                Engine::Instance()->dispatcher.trigger(Events::Load<Mesh>{entt::null, path});
            }

        }

        void on_load_mesh(const Events::Load<Mesh> &event) {
            entt::entity entity_id = event.entity_id;
            if (entity_id == entt::null) {
                entity_id = Engine::State().create();
            }
            SystemMesh::load(event.filepath, entity_id);
        }

        void on_construct(entt::registry &, entt::entity entity_id){
            Log::Info(fmt::format("Entity {} constructed component Mesh", static_cast<unsigned int>(entity_id))).enqueue();
        }

        void on_update(entt::registry &, entt::entity entity_id){
            Log::Info(fmt::format("Entity {} updated component Mesh", static_cast<unsigned int>(entity_id))).enqueue();
        }

        void on_destroy(entt::registry &, entt::entity entity_id){
            Log::Info(fmt::format("Entity {} removed component Mesh", static_cast<unsigned int>(entity_id))).enqueue();
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

    bool SystemMesh::load(const std::string &filepath, entt::entity entity) {
        MeshIo reader(filepath);
        Mesh mesh;
        if (!reader.read(mesh)) {
            Log::Warn("Failed to load mesh from file: " + filepath).enqueue();
            return false;
        } else {
            Log::Info("Loaded mesh from file: " + filepath).enqueue();
        }

        Engine::State().emplace<Mesh>(entity, mesh);
        return true;
    }

    void SystemMesh::pre_init() {

    }

    void SystemMesh::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemMeshInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemMeshInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemMesh::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemMeshInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemMeshInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}