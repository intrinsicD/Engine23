//
// Created by alex on 21.12.23.
//

#include "PluginComputeShaderNormals.h"
#include "Engine.h"
#include "Commands.h"
#include "Events/Events.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginComputeShaderNormalsInternal {
        void on_startup(const Events::Startup<Plugin> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginComputeShaderNormalsInternal {
        void on_startup(const Events::Startup<Plugin> &event) {

        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {

        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    PluginComputeShaderNormals::PluginComputeShaderNormals() : Plugin("compute_shader_normals") {

    }

    void PluginComputeShaderNormals::pre_init() {
        Engine::Dispatcher().sink<Events::Startup<Plugin>>().connect<&PluginComputeShaderNormalsInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&PluginComputeShaderNormalsInternal::on_shutdown>();
    }

    void PluginComputeShaderNormals::init() {
        Log::Info("Initialized", name()).enqueue();
    }

    void PluginComputeShaderNormals::remove() {
        Log::Info("Removed", name()).enqueue();
    }
}