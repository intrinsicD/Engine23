//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "Components.h"
#include "Systems.h"

namespace Bcg {
    void on_update_simulation_command_double_buffer(const Events::Update<SimulationCommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<SimulationCommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    void on_update_render_command_double_buffer(const Events::Update<RenderCommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<RenderCommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    void on_update_command_double_buffer(const Events::Update<CommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<CommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    Engine::Engine() {
        //The engine is a singleton
        //Has an entt::registry as state to store and represent the current state of the engine or application
        //Has an entt::dispatcher as dispatcher for decoupled communication between parts of the engine or application

        //The engine has a set of commands that are executed in a specific order
        //The command buffer is double buffered. It always contains the following commands:
        //ProcessUserCommands, UpdateStateCommand, RenderCommand, PostRenderCommand in that order.
        //Each command is Composite, it can contain other commands. This should be flexible enough for most cases, but can be of course extended if needed.

        //The main way to interact with the engine is via commands and events over the dispatcher or via direct state access.
        //The main way to extend the engines functionality is via plugins, systems and managers. But of course the engine can be extended in any way.

        entt::locator<Engine *>::emplace<Engine *>(this);
        dispatcher.sink<Events::Update<SimulationCommandDoubleBuffer>>().connect<&on_update_simulation_command_double_buffer>();
        dispatcher.sink<Events::Update<RenderCommandDoubleBuffer>>().connect<&on_update_render_command_double_buffer>();
        dispatcher.sink<Events::Update<CommandDoubleBuffer>>().connect<&on_update_command_double_buffer>();

        auto *command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<CommandDoubleBuffer>(command_buffer_current, command_buffer_next);

        auto *simulation_command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *simulation_command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<SimulationCommandDoubleBuffer>(simulation_command_buffer_current, simulation_command_buffer_next);

        auto *render_command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *render_command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<RenderCommandDoubleBuffer>(render_command_buffer_current, render_command_buffer_next);

        System::Timer::add_system();

        auto &time = Engine::Context().get<Time>();
        time.engine_constructor_start = Time::Point::Now();

        System::Logger::add_system();
        System::Renderer::add_system();
        System::Window::Glfw::add_system();
        System::Gui::add_system();
        System::UserInput::add_system();
        System::ParallelProcessing::add_system();

        time.engine_constructor_end = Time::Point::Now();
        Log::Info("Engine: Constructor took " + std::to_string(
                time.engine_constructor_end.duration<Time::Unit::seconds>(time.engine_constructor_start)) +
                  " seconds").enqueue();
    }

    Engine::~Engine() {
        System::ParallelProcessing::remove_system();
        System::UserInput::remove_system();
        System::Window::Glfw::remove_system();
        System::Logger::remove_system();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        entt::locator<Engine *>::reset();
    }

    void Engine::run() {
        auto &time = Engine::Context().get<Time>();
        time.engine_run_start = Time::Point::Now();
        is_running = true;
        //The primary workflow is this:
        //1. notify all systems of an event
        //2. systems can add commands to the command buffer
        //3. execute the command buffer
        //4. repeat

        //startup engine
        Log::Info("Engine: Startup...").enqueue();
        dispatcher.trigger<Events::Startup<Engine>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();

        Log::Info("Engine: Run...").enqueue();
        time.mainloop.current = Time::Point::Now();
        time.simulationloop.avg_duration = time.simulationloop.min_step_size;
        time.simulationloop.duration = time.simulationloop.min_step_size;

        while (is_running) {
            dispatcher.trigger<Events::Begin<MainLoop>>();
            dispatcher.trigger<Events::Update<Input>>();

            while (time.simulationloop.accumulator > time.simulationloop.duration) {
                dispatcher.trigger<Events::Begin<SimulationLoop>>();

                dispatcher.trigger<Events::Update<SimulationLoop>>();
                dispatcher.trigger<Events::Update<SimulationCommandDoubleBuffer>>();

                dispatcher.trigger<Events::End<SimulationLoop>>();
            }

            dispatcher.trigger<Events::Begin<Frame>>();
            dispatcher.trigger<Events::Render<Frame>>();
            dispatcher.trigger<Events::Render<Gui>>();
            dispatcher.trigger<Events::Update<RenderCommandDoubleBuffer>>();
            dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
            dispatcher.trigger<Events::End<Frame>>();
            dispatcher.trigger<Events::End<MainLoop>>();
        }

        //shutdown engine
        Log::Info("Engine: Shutdown...").enqueue();
        dispatcher.trigger<Events::Shutdown<Engine>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        time.engine_run_end = Time::Point::Now();
    }
}