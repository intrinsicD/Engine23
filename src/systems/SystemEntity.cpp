//
// Created by alex on 04.01.24.
//

#include "systems/SystemEntity.h"
#include "Engine.h"
#include "Events.h"
#include "imgui.h"
#include "fmt/core.h"
#include "Commands.h"
#include "AABBGui.h"
#include "components/Transform.h"
#include "components/Picker.h"
#include "components/EntityName.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemEntityInternal {
        static bool show_gui = false;

        void on_create(const Events::Create<entt::entity> &event);

        void on_destroy(const Events::Destroy<entt::entity> &event);

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
    namespace SystemEntityInternal {
        void on_create(const Events::Create<entt::entity> &event) {
            if (event.return_value != nullptr) {
                auto &picker = Engine::Context().get<Picker>();
                *event.return_value = SystemEntity::create_entity();
                picker.id.entity =  *event.return_value;
                Log::Info(fmt::format("Entity {} created", static_cast<size_t>(*event.return_value))).enqueue();
            } else {
                Log::Error("Entity creation failed, event return pointer is nullptr").enqueue();
            }
        }

        void on_destroy(const Events::Destroy<entt::entity> &event) {
            SystemEntity::destroy_entity(*event.value);
            Log::Info(fmt::format("Entity {} destroyed", static_cast<size_t>(*event.value))).enqueue();
        }

        void show_gui_entity(entt::entity entity) {
            if (ImGui::BeginTabBar("Entity")) {
                if (ImGui::BeginTabItem("Info")) {
                    ImGui::Text("ID: %zu", static_cast<size_t>(entity));
                    if(Engine::State().all_of<std::string>(entity)){
                        ImGui::Text("Name: %s", Engine::State().get<std::string>(entity).c_str());
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Components")) {
                    if(Engine::State().all_of<EntityName>(entity)){
                        if(ImGui::CollapsingHeader("Name")){
                            ComponentGui<EntityName>::Show(entity);
                        }
                    }
                    if(Engine::State().all_of<AABB3>(entity)){
                        if(ImGui::CollapsingHeader("AABB")){
                            ComponentGui<AABB3>::Show(entity);
                        }
                    }
                    if(Engine::State().all_of<Transform>(entity)){
                        if(ImGui::CollapsingHeader("Transform")){
                            ComponentGui<Transform>::Show(entity);
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Hierarchy")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Transform")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Geometry")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Topology")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Material")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Texture")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Animation")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Light")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Camera")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Render")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Simulation")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Script")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Audio")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Physics")) {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Misc")) {
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemEntityInternal::on_render_gui>();
                return;
            }

            if (ImGui::Begin("Entity", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                if (ImGui::Button("Create")) {
                    Engine::Instance()->dispatcher.trigger(Events::Create<entt::entity>{&picker.id.entity});
                }
                ImGui::SameLine();
                if (ImGui::Button("Destroy")) {
                    Engine::Instance()->dispatcher.trigger(Events::Destroy<entt::entity>{&picker.id.entity});
                    picker.id.entity = entt::null;
                }
                if (picker.id.entity != entt::null) {
                    show_gui_entity(picker.id.entity);
                }
            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Entity", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemEntityInternal::on_render_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemEntityInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Create<entt::entity>>().connect<&SystemEntityInternal::on_create>();
            Engine::Instance()->dispatcher.sink<Events::Destroy<entt::entity>>().connect<&SystemEntityInternal::on_destroy>();
            Log::Info(SystemEntity::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Log::Info(SystemEntity::name(), "Shutdown").enqueue();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemEntityInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Create<entt::entity>>().connect<&SystemEntityInternal::on_create>();
            Engine::Instance()->dispatcher.sink<Events::Destroy<entt::entity>>().connect<&SystemEntityInternal::on_destroy>();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemEntity::name() {
        return "SystemEntity";
    }

    entt::entity SystemEntity::create_entity() {
        return Engine::State().create();
    }

    void SystemEntity::destroy_entity(entt::entity entity) {
        Engine::State().destroy(entity);
    }

    void SystemEntity::set_name(entt::entity entity, std::string name) {
        if(Engine::State().all_of<EntityName>(entity)){
            Engine::State().remove<EntityName>(entity);
        }
        Engine::State().emplace<EntityName>(entity, name);
    }

    void SystemEntity::pre_init() {
        Engine::Context().emplace<Picker>();
    }

    void SystemEntity::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemEntityInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemEntityInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemEntity::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemEntityInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemEntityInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}