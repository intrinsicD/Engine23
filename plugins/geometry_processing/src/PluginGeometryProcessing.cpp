//
// Created by alex on 15.12.23.
//

#include "PluginGeometryProcessing.h"
#include "Engine.h"
#include "Events.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginGeometryProcessingInternal {
        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_startup(const Events::Startup<Plugin> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginGeometryProcessingInternal {
        void on_render_gui_menu(const Events::Render<GuiMenu> &event){
            if (ImGui::BeginMenu("Geometry Processing")) {
                ImGui::EndMenu();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event){

        }

        void on_shutdown(const Events::Shutdown<Engine> &event){

        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    PluginGeometryProcessing::PluginGeometryProcessing() : Plugin("geometry_processing") {}

    void PluginGeometryProcessing::pre_init() {
        //register necessary components
    }

    void PluginGeometryProcessing::init() {
        //register event handlers
    }

    void PluginGeometryProcessing::remove() {
        //unregister event handlers
    }
}