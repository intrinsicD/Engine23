//
// Created by alex on 04.01.24.
//

#include "systems/SystemEntity.h"
#include "Engine.h"
#include "Events/Events.h"
#include "imgui.h"
#include "fmt/core.h"
#include "Commands.h"
#include "Transform/Transform.h"
#include "Picker.h"
#include "Asset.h"
#include "Input.h"
#include "EntityGui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemEntityInternal {
        static bool show_gui = false;

        void on_create(const Events::Create<entt::entity> &event);

        void on_destroy(const Events::Destroy<entt::entity> &event);

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

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemEntityInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Entity", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                if (ImGui::Button("Create")) {
                    Engine::Dispatcher().trigger(Events::Create<entt::entity>{&picker.id.entity});
                }
                ImGui::SameLine();
                if (ImGui::Button("Destroy")) {
                    Engine::Dispatcher().trigger(Events::Destroy<entt::entity>{&picker.id.entity});
                    picker.id.entity = entt::null;
                }
                if (picker.id.entity != entt::null) {
                    ComponentGui<Entity>::Show(picker.id.entity);
                }
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Entity", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemEntityInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemEntityInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Create<entt::entity>>().connect<&SystemEntityInternal::on_create>();
            Engine::Dispatcher().sink<Events::Destroy<entt::entity>>().connect<&SystemEntityInternal::on_destroy>();
            Log::Info(SystemEntity::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Log::Info(SystemEntity::name(), "Shutdown").enqueue();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemEntityInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Create<entt::entity>>().connect<&SystemEntityInternal::on_create>();
            Engine::Dispatcher().sink<Events::Destroy<entt::entity>>().connect<&SystemEntityInternal::on_destroy>();
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
        if(!Engine::State().all_of<Asset>(entity)){
            Engine::State().emplace<Asset>(entity);
        }

        auto &asset = Engine::State().get<Asset>(entity);
        asset.name = name;
    }

    void SystemEntity::pre_init() {

    }

    void SystemEntity::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemEntityInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemEntityInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemEntity::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemEntityInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemEntityInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}