//
// Created by alex on 01.12.23.
//

#include "systems/SystemPlugins.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "Plugins.h"


#include <dlfcn.h>

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemPluginsInternal {
        struct PluginsConfig {
            std::string plugin_prefix = "libbcg_plugin_";
            std::string plugin_ext = ".so";
            std::string create_instance_prefix = "create_instance_";

            std::vector<std::pair<Plugin *, void *>> plugins;
        };

        void on_startup(Events::Startup<Engine> &event);

        void on_shutdown(Events::Shutdown<Engine> &event);

        inline std::string extract_plugin_name(const std::string &prefix,
                                               const std::string &filepath, const std::string &ext);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemPluginsInternal {
        void on_startup(Events::Startup<Engine> &event) {
            Log::Info(SystemPlugins::name() , "Startup").enqueue();
        }

        void on_shutdown(Events::Shutdown<Engine> &event) {
            Log::Info(SystemPlugins::name() , "Shutdown").enqueue();
        }

        inline std::string
        extract_plugin_name(const std::string &prefix, const std::string &filepath, const std::string &ext) {
            size_t start = filepath.find(prefix);
            if (start == std::string::npos) {
                return ""; // Prefix not found
            }
            start += prefix.length(); // Move past the prefix
            size_t end = filepath.rfind(ext);
            if (end == std::string::npos) {
                return ""; // ".so" not found
            }

            return filepath.substr(start, end - start);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemPlugins::name() {
        return "SystemPlugins";
    }

    Plugin *SystemPlugins::load(const std::string &filepath) {
        void *handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (handle) {
            dlerror();
            auto &config = Engine::Context().get<SystemPluginsInternal::PluginsConfig>();
            auto create_instance_name = config.create_instance_prefix +
                                        SystemPluginsInternal::extract_plugin_name(config.plugin_prefix, filepath,
                                                                                   config.plugin_ext);
            auto create = (Plugin *(*)()) dlsym(handle, create_instance_name.c_str());
            if (create) {
                const char *dlsym_error = dlerror();
                if (!dlsym_error) {
                    return create();
                }
            }
        }
        dlclose(handle);
        return nullptr;
    }

    void SystemPlugins::unload(void *handle) {
        dlclose(handle);
    }

    void SystemPlugins::pre_init() {
        Engine::Context().emplace<SystemPluginsInternal::PluginsConfig>();
    }

    void SystemPlugins::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemPluginsInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemPluginsInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();

        auto &plugins = Engine::Context().emplace<std::unordered_map<std::string, Plugin *>>();

/*        {
            auto plugin = load("lib/libbcg_plugin_tiny_obj_loader.so");
            plugin->pre_init();
            plugin->init();
            plugins["tiny_obj_loader"] = plugin;
        }*/
    }

    void SystemPlugins::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemPluginsInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemPluginsInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();

        auto &plugins = Engine::Context().emplace<std::unordered_map<std::string, Plugin *>>();
        for (auto &plugin: plugins) {
            plugin.second->remove();
            unload(plugin.second);
        }
    }
}
