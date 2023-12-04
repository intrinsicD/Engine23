//
// Created by alex on 04.12.23.
//

#include "SystemPlatform.h"
#include "Events.h"
#include "Engine.h"
#include "Commands.h"
#include "imgui.h"

namespace Bcg {
    struct PlatformInfo {
        std::string os;
        std::string arch;
        std::string compiler;
    };

    namespace SystemPlatformInternal {
        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemPlatformInternal::on_render_gui>();
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

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Platform")) {
                if (ImGui::MenuItem("Platform", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemPlatformInternal::on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemPlatformInternal::on_render_gui_menu>();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemPlatformInternal::on_render_gui_menu>();
        }
    }

    SystemPlatform::SystemPlatform() : System("SystemPlatform") {

    }

    void SystemPlatform::pre_init() {
        auto &info = Engine::Context().emplace<PlatformInfo>();

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
        info.compiler += " " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
#else
        info.compiler = "Unknown";
#endif

    }

    void SystemPlatform::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemPlatformInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemPlatformInternal::on_shutdown>();
        auto &info = Engine::Context().emplace<PlatformInfo>();
        Log::Info(m_name + ": Initialized. Compiled with " + info.compiler + " on " + info.os + " for " + info.arch +
                  " Architecture.").enqueue();
    }

    void SystemPlatform::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemPlatformInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemPlatformInternal::on_shutdown>();
        Log::Info(m_name + ": Removed").enqueue();
    }
}