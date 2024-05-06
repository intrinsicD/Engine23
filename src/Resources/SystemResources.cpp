//
// Created by alex on 02.05.24.
//

#include "SystemResources.h"
#include "Engine.h"
#include "Commands.h"
#include "Events.h"
#include "imgui.h"
#include <typeindex>

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemResourcesInternal {
        static bool show_gui = false;

        struct ExistingContainers {
            std::unordered_map<std::string, std::type_index> existing_containers;

            bool has(const std::string &name) {
                return existing_containers.find(name) != existing_containers.end();
            }

            bool has(const std::type_index &index) {
                for (const auto &item: existing_containers) {
                    if (item.second == index) return true;
                }
                return false;
            }
        };

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemResourcesInternal {
        void on_startup_engine(const Events::Startup<Engine> &event) {
            Log::Info("Startup " + SystemResources::name()).enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Log::Info("Shutdown " + SystemResources::name()).enqueue();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Resources", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemResourcesInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemResourcesInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Resources", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {


            }
            ImGui::End();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemResources::name() {
        return "SystemResources";
    }

    void SystemResources::pre_init() {
        Engine::Context().emplace<SystemResourcesInternal::ExistingContainers>();
    }

    void SystemResources::init() {
        Log::Info("Initialized " + name()).enqueue();
    }

    void SystemResources::remove() {
        Log::Info("Removed " + name()).enqueue();
    }
}