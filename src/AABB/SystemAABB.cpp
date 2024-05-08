//
// Created by alex on 23.02.24.
//

#include "SystemAABB.h"
#include "imgui.h"
#include "Commands.h"
#include "Events/Events.h"
#include "Engine.h"
#include "Entity.h"
#include "AABBGui.h"
#include "PropertyEigenMap.h"
#include "fmt/core.h"
#include "Picker.h"
#include "SystemsUtils.h"
#include "Components.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAABBInternal {
        static bool show_gui = false;

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_aabb(const Events::Update<AABB3, entt::entity> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAABBInternal {
        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemAABBInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Update<AABB3, entt::entity>>().connect<&SystemAABBInternal::on_update_aabb>();
            Engine::State().on_construct<AABB3>().connect<&on_construct_component<SystemAABB>>();
            Engine::State().on_update<AABB3>().connect<&on_update_component<SystemAABB>>();
            Engine::State().on_destroy<AABB3>().connect<&on_destroy_component<SystemAABB>>();
            Log::Info(SystemAABB::name(), "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemAABBInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Update<AABB3, entt::entity>>().disconnect<&SystemAABBInternal::on_update_aabb>();
            Engine::State().on_construct<AABB3>().disconnect<&on_construct_component<SystemAABB>>();
            Engine::State().on_update<AABB3>().disconnect<&on_update_component<SystemAABB>>();
            Engine::State().on_destroy<AABB3>().disconnect<&on_destroy_component<SystemAABB>>();
            Log::Info(SystemAABB::name(), "Shutdown").enqueue();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("AABB", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("AABB", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<AABB3>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_aabb(const Events::Update<AABB3, entt::entity> &event) {
            auto entity_id = std::get<0>(event.data);
            Entity entity(Engine::State(), entity_id);
            if (!entity.is_valid()) {
                return;
            }
            auto *vertices = entity.vertices();
            Property<Eigen::Vector<double, 3>> positions;
            if (vertices) {
                positions = vertices->get<Eigen::Vector<double, 3>>("v_position");
                if (!positions) {
                    Log::Warn(fmt::format("Entity {} has no position data",
                                          static_cast<unsigned int>(entity_id))).enqueue();
                    return;
                }
            } else {
                Log::Warn(fmt::format("Entity {} has no vertices", static_cast<unsigned int>(entity_id))).enqueue();
                return;
            }

            Components<AABB3> components(SystemAABB::component_name());

            if (!entity.all_of<Component<AABB3>>()) {
                auto aabb_id = components.create_instance();
                components.add_to_entity(entity_id, aabb_id);
                components.get_instance(aabb_id).fit(MapConst(positions));
            } else {
                auto &component = Engine::State().get<Component<AABB3>>(entity_id);
                components.get_instance(component.index).fit(MapConst(positions));
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemAABB::name() {
        return "SystemAABB";
    }

    std::string SystemAABB::component_name() {
        return "AABB";
    }

    void SystemAABB::pre_init() {

    }

    void SystemAABB::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemAABBInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemAABBInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemAABB::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemAABBInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemAABBInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}