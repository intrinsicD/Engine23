//
// Created by alex on 29.11.23.
//

#include "SystemTimer.h"
#include "Engine.h"
#include "Components.h"
#include "Commands.h"
#include "imgui.h"

namespace Bcg::System::Timer {
    void add_system() {
        Engine::Context().emplace<Time>();
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
    }

    void remove_system() {
        Engine::Context().erase<Time>();
    }

    void begin_main_loop(Time &time) {
        time.simulationloop.iter_counter = 0;
        time.simulationloop.accumulator += time.mainloop.duration;
    }

    void end_main_loop(Time &time) {
        time.mainloop.last = time.mainloop.current;
        time.mainloop.current = Time::Point::Now();
        time.mainloop.duration = time.mainloop.current.duration<Time::Unit::seconds>(time.mainloop.last);
        time.mainloop.avg_duration = time.mainloop.avg_duration * time.mainloop.iter_counter + time.mainloop.duration;
        time.mainloop.avg_duration /= ++time.mainloop.iter_counter;
        time.mainloop.fps = 1.0f / time.mainloop.duration;
        time.mainloop.avg_fps = 1.0f / time.mainloop.avg_duration;
    }

    void begin_simulation_loop(Time &time) {
        time.simulationloop.start = Time::Point::Now();
    }

    void end_simulation_loop(Time &time) {
        time.simulationloop.duration = Time::Point::Now().duration<Time::Unit::seconds>(
                time.simulationloop.start);
        time.simulationloop.avg_duration = time.simulationloop.avg_duration * time.simulationloop.iter_counter +
                                           time.simulationloop.duration;
        time.simulationloop.avg_duration /= ++time.simulationloop.iter_counter;

        time.simulationloop.accumulator -= std::max(time.simulationloop.duration,
                                                    time.simulationloop.min_step_size);
    }

    void on_startup(const Events::Startup<Engine> &event) {
        auto &time = Engine::Context().get<Time>();
        time.engine_run_start = Time::Point::Now();
        time.mainloop.current = Time::Point::Now();
        time.simulationloop.avg_duration = time.simulationloop.min_step_size;
    }

    static bool show_gui = true;

    void render_gui(Time &time) {
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

    void on_begin_frame(const Events::Begin<Frame> &event) {
/*        Engine::Context().get<CommandDoubleBuffer>().enqueue_next(
                std::make_shared<TaskCommand>("UpdateSystemTimer", [&]() {
                    update_system();
                    return 1;
                }));*/

        Engine::Context().get<CommandDoubleBuffer>().enqueue_next(
                std::make_shared<TaskCommand>("RenderMenu", [&]() {
                    if (ImGui::BeginMenu("Menu")) {
                        ImGui::MenuItem("Timer", nullptr, &show_gui);
                        ImGui::EndMenu();
                    }
                    return 1;
                }));

        if (show_gui) {
            Engine::Context().get<CommandDoubleBuffer>().enqueue_next(
                    std::make_shared<TaskCommand>("RenderGui", [&]() {
                        if (ImGui::Begin("Timer", &show_gui)) {
                            render_gui(Engine::Context().get<Time>());
                        }
                        ImGui::End();
                        return 1;
                    }));
        }
    }
}