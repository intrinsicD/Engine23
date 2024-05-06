//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemBuffersInternal {
        static bool show_gui = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemBuffersInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Buffers", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event){
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Buffers", &show_gui)){

            }
            ImGui::End();
        }

        void on_startup_engine(const Events::Startup<Engine> &event){
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemBuffersInternal::on_update_gui_menu>();
            Log::Info(SystemBuffers::name() , "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event){
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemBuffersInternal::on_update_gui_menu>();
            Log::Info(SystemBuffers::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemBuffers::name() {
        return "SystemBuffers";
    }

    void SystemBuffers::pre_init() {

    }

    void SystemBuffers::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemBuffersInternal::on_startup_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemBuffers::remove() {
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemBuffersInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}