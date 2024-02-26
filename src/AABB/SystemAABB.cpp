//
// Created by alex on 23.02.24.
//

#include "SystemAABB.h"
#include "imgui.h"
#include "Commands.h"
#include "Events.h"
#include "Engine.h"
#include "components/Entity.h"
#include "AABBGui.h"
#include "PropertyEigenMap.h"
#include "fmt/core.h"
#include "components/Picker.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAABBInternal {
        static bool show_gui = false;

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_render_gui(const Events::Render<Gui> &event);

        void on_construct(entt::registry &, entt::entity);

        void on_update(entt::registry &, entt::entity);

        void on_destroy(entt::registry &, entt::entity);

        void on_update_aabb(const Events::Update<AABB3> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemAABBInternal {
        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemAABBInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Update<AABB3>>().connect<&SystemAABBInternal::on_update_aabb>();
            Engine::State().on_construct<AABB3>().connect<&SystemAABBInternal::on_construct>();
            Engine::State().on_update<AABB3>().connect<&SystemAABBInternal::on_update>();
            Engine::State().on_destroy<AABB3>().connect<&SystemAABBInternal::on_destroy>();
            Log::Info(SystemAABB::name() , "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemAABBInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Update<AABB3>>().disconnect<&SystemAABBInternal::on_update_aabb>();
            Engine::State().on_construct<AABB3>().disconnect<&SystemAABBInternal::on_construct>();
            Engine::State().on_update<AABB3>().disconnect<&SystemAABBInternal::on_update>();
            Engine::State().on_destroy<AABB3>().disconnect<&SystemAABBInternal::on_destroy>();
            Log::Info(SystemAABB::name() , "Shutdown").enqueue();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("AABB", nullptr, &show_gui)) {
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

            if (ImGui::Begin("AABB", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<AABB3>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_construct(entt::registry &registry, entt::entity entity_id) {
            Log::Info(fmt::format("Entity {} constructed component AABB",
                                  static_cast<unsigned int>(entity_id))).enqueue();
        }

        void on_update(entt::registry &, entt::entity entity_id) {
            Log::Info(fmt::format("Entity {} updated component AABB", static_cast<unsigned int>(entity_id))).enqueue();
        }

        void on_destroy(entt::registry &, entt::entity entity_id) {
            Log::Info(fmt::format("Entity {} removed component AABB", static_cast<unsigned int>(entity_id))).enqueue();
        }

        void on_update_aabb(const Events::Update<AABB3> &event) {
            Entity entity(Engine::State(), event.entity_id);
            if(!entity.is_valid()){
                return;
            }
            auto *vertices = entity.vertices();
            Property<Eigen::Vector<double, 3>> positions;
            if(vertices){
                positions = vertices->get<Eigen::Vector<double, 3>>("v_position");
                if(!positions){
                    Log::Warn(fmt::format("Entity {} has no position data", static_cast<unsigned int>(event.entity_id))).enqueue();
                    return;
                }
            }else{
                Log::Warn(fmt::format("Entity {} has no vertices", static_cast<unsigned int>(event.entity_id))).enqueue();
                return;
            }

            AABB3 aabb;
            aabb.fit(MapConst(positions));

            if (!entity.all_of<AABB3>()) {
                entity.emplace<AABB3>(std::move(aabb));
            }else{
                entity.replace<AABB3>(std::move(aabb));
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

    void SystemAABB::pre_init() {

    }

    void SystemAABB::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemAABBInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemAABBInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemAABB::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemAABBInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemAABBInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}