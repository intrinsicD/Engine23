//
// Created by alex on 01.12.23.
//

#include "SystemPlugins.h"
#include "Engine.h"
#include "Commands.h"

#include <dlfcn.h>

namespace Bcg::System::Plugins {
    struct PluginsConfig {
        std::string plugin_prefix = "libbcg_plugin_";
        std::string plugin_ext = ".so";
        std::string create_instance_prefix = "create_instance_";
    };

    void pre_init_system() {
        Engine::Context().emplace<PluginsConfig>();
    }

    void init_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&on_shutdown>();
        Log::Info("SystemPlugins: Initialized").enqueue();
    }

    void remove_system() {
        Log::Info("SystemPlugins: Removed").enqueue();
    }

    void on_startup(Events::Startup<Engine> &event) {
        Log::Info("SystemPlugins: Startup").enqueue();
    }

    void on_shutdown(Events::Shutdown<Engine> &event) {
        Log::Info("SystemPlugins: Shutdown").enqueue();
    }

    std::string extract_plugin_name(const std::string &prefix, const std::string &filepath, const std::string &ext) {
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

    Plugin *load(const std::string &filepath) {
        void *handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (handle) {
            dlerror();
            auto &config = Engine::Context().get<PluginsConfig>();
            auto create_instance_name = config.create_instance_prefix +
                                        extract_plugin_name(config.plugin_prefix, filepath, config.plugin_ext);
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

    void unload(void *handle) {
        dlclose(handle);
    }
}
