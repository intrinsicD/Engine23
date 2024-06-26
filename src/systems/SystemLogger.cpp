//
// Created by alex on 27.11.23.
//

#include "systems/SystemLogger.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemLoggerInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);


        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemLoggerInternal {
        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemLoggerInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Logger", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                int logLevelInt = static_cast<int>(Engine::Context().get<LogLevel>());
                if (ImGui::RadioButton("Error", &logLevelInt, 0)) {
                    SystemLogger::set_log_level(LogLevel::Error);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Warning", &logLevelInt, 1)) {
                    SystemLogger::set_log_level(LogLevel::Warn);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Info", &logLevelInt, 2)) {
                    SystemLogger::set_log_level(LogLevel::Info);
                }
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Logger", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemLoggerInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }


        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemLoggerInternal::on_update_gui_menu>();
            Log::Info(SystemLogger::name() + "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemLoggerInternal::on_update_gui_menu>();
            Log::Info(SystemLogger::name() + "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemLogger::name() {
        return "SystemLogger";
    }

    void SystemLogger::pre_init() {
        Engine::Context().emplace<LogLevel>(LogLevel::Info);
    }

    void SystemLogger::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemLoggerInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemLoggerInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
        set_log_level(LogLevel::Info);
    }

    void SystemLogger::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemLoggerInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemLoggerInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
        Log::TODO(name() + ": figure out enabling and disabling logging").enqueue();
    }

    void SystemLogger::set_log_level(LogLevel level) {
        auto &logLevel = Engine::Context().get<LogLevel>();
        logLevel = level;
        std::string level_str;
        switch (level) {
            case LogLevel::Info:
                level_str = "Info";
                break;
            case LogLevel::Warn:
                level_str = "Warning";
                break;
            case LogLevel::Error:
                level_str = "Error";
                break;
        }
        Log::Always(name() + ": Set LogLevel to " + level_str).enqueue();
    }
}