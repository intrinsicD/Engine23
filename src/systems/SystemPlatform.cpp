//
// Created by alex on 04.12.23.
//

#include "systems/SystemPlatform.h"
#include "Events/Events.h"
#include "Engine.h"
#include "Commands.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemPlatformInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemPlatformInternal {
        struct PlatformInfo {
            std::string os;
            std::string arch;
            std::string compiler;
        };

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemPlatformInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Platform", &show_gui)) {
                auto &platform_info = Engine::Context().get<PlatformInfo>();
                ImGui::Text("OS: %s", platform_info.os.c_str());
                ImGui::Text("Arch: %s", platform_info.arch.c_str());
                ImGui::Text("Compiler: %s", platform_info.compiler.c_str());
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Platform")) {
                if (ImGui::MenuItem("Platform", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemPlatformInternal::on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemPlatformInternal::on_update_gui_menu>();
            Log::Info(SystemPlatform::name() , "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemPlatformInternal::on_update_gui_menu>();
            Log::Info(SystemPlatform::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemPlatform::name() {
        return "SystemPlatform";
    }

    void SystemPlatform::pre_init() {
        auto &info = Engine::Context().emplace<SystemPlatformInternal::PlatformInfo>();

        // Detect Operating System
#if defined(_WIN32) || defined(_WIN64)
        info.os = "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
        info.os = "macOS";
#elif defined(__linux__)
        info.os = "Linux";
#else
        info.os = "Unknown";
#endif

        // Detect Architecture
#if defined(__x86_64__) || defined(_M_X64)
        info.arch = "64-bit";
#elif defined(__i386__) || defined(_M_IX86)
        info.arch = "32-bit";
#else
    info.arch = "Unknown";
#endif

        // Compiler specific code
#ifdef _MSC_VER
        info.compiler = "MSVC";
        info.compiler += " " + std::to_string(_MSC_VER);
#elif defined(__clang__)
        info.compiler = "Clang";
        info.compiler += " " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." +
                         std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
        info.compiler = "GCC";
        info.compiler += " " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." +
                         std::to_string(__GNUC_PATCHLEVEL__);
#else
        info.compiler = "Unknown";
#endif

    }

    void SystemPlatform::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemPlatformInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemPlatformInternal::on_shutdown>();
        auto &info = Engine::Context().emplace<SystemPlatformInternal::PlatformInfo>();
        Log::Info(name() + ": Initialized. Compiled with " + info.compiler + " on " + info.os + " for " + info.arch +
                  " Architecture.").enqueue();
    }

    void SystemPlatform::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemPlatformInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemPlatformInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}