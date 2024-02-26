//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemBuffersInternal {
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
    namespace SystemBuffersInternal {
        void on_render_gui_menu(const Events::Render<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Buffers", nullptr, &show_gui)) {
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

            if (ImGui::Begin("Buffers", &show_gui)){

            }
            ImGui::End();
        }

        void on_startup_engine(const Events::Startup<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemBuffersInternal::on_render_gui_menu>();
            Log::Info(SystemBuffers::name() , "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemBuffersInternal::on_render_gui_menu>();
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
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemBuffersInternal::on_startup_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemBuffers::remove() {
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemBuffersInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}