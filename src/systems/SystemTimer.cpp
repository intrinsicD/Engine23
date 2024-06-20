//
// Created by alex on 29.11.23.
//

#include "systems/SystemTimer.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "imgui.h"
#include "Time.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTimerInternal {
        void on_startup(const Events::Startup<Engine> &event);

        void on_begin_simulation_loop(const Events::Begin<SimulationLoop> &event);

        void on_end_simulation_loop(const Events::End<SimulationLoop> &event);

        void on_begin_main_loop(const Events::Begin<MainLoop> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_end_main_loop(const Events::End<MainLoop> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTimerInternal {
        void on_startup(const Events::Startup<Engine> &event) {
            auto &time = Engine::Context().get<Time>();
            time.engine_run_start = Time::Point::Now();
            time.mainloop.current = Time::Point::Now();
            time.simulationloop.avg_duration = time.simulationloop.min_step_size;
            Log::Info(SystemTimer::name(), "Startup").enqueue();
        }

        void on_begin_simulation_loop(const Events::Begin<SimulationLoop> &event) {
            auto &time = Engine::Context().get<Time>();
            time.simulationloop.start = Time::Point::Now();
        }

        void on_end_simulation_loop(const Events::End<SimulationLoop> &event) {
            auto &time = Engine::Context().get<Time>();
            time.simulationloop.duration = Time::Point::Now().duration<Time::Unit::seconds>(
                    time.simulationloop.start);
            time.simulationloop.avg_duration = time.simulationloop.avg_duration * time.simulationloop.iter_counter +
                                               time.simulationloop.duration;
            time.simulationloop.avg_duration /= ++time.simulationloop.iter_counter;
            time.simulationloop.duration = std::max(time.simulationloop.duration,
                                                    time.simulationloop.min_step_size);
            time.simulationloop.accumulator -= time.simulationloop.duration;
        }

        void on_begin_main_loop(const Events::Begin<MainLoop> &event) {
            auto &time = Engine::Context().get<Time>();
            time.simulationloop.iter_counter = 0;
            time.simulationloop.accumulator += time.mainloop.duration;
        }

        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Timer", &show_gui)) {
                auto &time = Engine::Context().get<Time>();
                ImGui::Text("Mainloop: %f", time.mainloop.duration);
                ImGui::Text("Avg: %f", time.mainloop.avg_duration);
                ImGui::Text("FPS: %d", time.mainloop.fps);
                ImGui::Text("Avg FPS: %d", time.mainloop.avg_fps);
                ImGui::Text("Iter: %zu", time.mainloop.iter_counter);
                ImGui::Separator();
                ImGui::Text("Sim. duration: %f", time.simulationloop.duration);
                ImGui::Text("Sim. avg duration: %f", time.simulationloop.avg_duration);
                ImGui::Text("Sim. Iter: %zu", time.simulationloop.iter_counter);
                auto min_step_size = 1.0 / time.simulationloop.min_step_size;
                if (ImGui::InputDouble("Sim. min_step_size (Hz):", &min_step_size)) {
                    if (min_step_size > 0 && min_step_size < 3000001) {
                        time.simulationloop.min_step_size = 1.0 / min_step_size;
                    }
                }
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Timer", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_end_main_loop(const Events::End<MainLoop> &event) {
            auto &time = Engine::Context().get<Time>();
            time.mainloop.last = time.mainloop.current;
            time.mainloop.current = Time::Point::Now();
            time.mainloop.duration = time.mainloop.current.duration<Time::Unit::seconds>(time.mainloop.last);
            time.mainloop.avg_duration =
                    time.mainloop.avg_duration * time.mainloop.iter_counter + time.mainloop.duration;
            time.mainloop.avg_duration /= ++time.mainloop.iter_counter;
            time.mainloop.fps = static_cast<int>(1.0 / time.mainloop.duration);
            time.mainloop.avg_fps = static_cast<int>(1.0 / time.mainloop.avg_duration);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTimer::name() {
        return "SystemTimer";
    }

    void SystemTimer::pre_init() {
        Engine::Context().emplace<Time>();
    }

    void SystemTimer::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemTimerInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemTimerInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Begin<SimulationLoop>>().connect<&SystemTimerInternal::on_begin_simulation_loop>();
        Engine::Dispatcher().sink<Events::End<SimulationLoop>>().connect<&SystemTimerInternal::on_end_simulation_loop>();
        Engine::Dispatcher().sink<Events::Begin<MainLoop>>().connect<&SystemTimerInternal::on_begin_main_loop>();
        Engine::Dispatcher().sink<Events::End<MainLoop>>().connect<&SystemTimerInternal::on_end_main_loop>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemTimer::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemTimerInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemTimerInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Begin<SimulationLoop>>().disconnect<&SystemTimerInternal::on_begin_simulation_loop>();
        Engine::Dispatcher().sink<Events::End<SimulationLoop>>().disconnect<&SystemTimerInternal::on_end_simulation_loop>();
        Engine::Dispatcher().sink<Events::Begin<MainLoop>>().disconnect<&SystemTimerInternal::on_begin_main_loop>();
        Engine::Dispatcher().sink<Events::End<MainLoop>>().disconnect<&SystemTimerInternal::on_end_main_loop>();
        Log::Info("Removed", name()).enqueue();
    }
}