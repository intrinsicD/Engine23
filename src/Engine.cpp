//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "Factories.h"
#include "Platform.h"
#include "Systems.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "vulkan/vulkan.h"

#include <iostream>

namespace Bcg {
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
        auto &time = Engine::Context().emplace<Time>();
        time.engine_constructor_start = Time::Point::Now();

        auto *current = &Engine::Context().emplace<CommandBufferCurrent>();
        auto *next = &Engine::Context().emplace<CommandBufferNext>();
        Engine::Context().emplace<CommandDoubleBuffer>(current, next);

        System::Logger::add_system();
        System::Renderer::add_system();
        System::Window::Glfw::add_system();
        System::Gui::add_system();
        System::UserInput::add_system();

        time.engine_constructor_end = Time::Point::Now();
        Log::Info("Engine: Constructor took " + std::to_string(time.engine_constructor_end.duration<Time::Unit::seconds>(time.engine_constructor_start)) + " seconds");
    }

    Engine::~Engine() {
        System::UserInput::remove_system();
        System::Window::Glfw::remove_system();
        System::Logger::remove_system();

        CommandBuffer *command_buffer_current = &state.ctx().get<CommandBufferCurrent>();
        CommandBuffer *command_buffer_next = &state.ctx().get<CommandBufferNext>();
        //execute command buffers for shutdown
        for (auto &command: *command_buffer_current) {
            command->execute();
        }
        for (auto &command: *command_buffer_next) {
            command->execute();
        }

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
        Log::Info("Engine: Startup...");
        dispatcher.trigger<Events::Startup<Engine>>();

        //execute command buffers for startup
        auto &double_buffer = Engine::Context().get<CommandDoubleBuffer>();

        for (auto &command: *double_buffer.current) {
            command->execute();
        }

        double_buffer.current->clear();
        double_buffer.current->swap(*double_buffer.next);

        Log::Info("Engine: Run...");
        time.mainloop.current = Time::Point::Now();
        while (is_running) {
            time.mainloop.last = time.mainloop.current;
            time.mainloop.current = Time::Point::Now();
            time.mainloop.duration = time.mainloop.current.duration<Time::Unit::seconds>(time.mainloop.last);
            time.mainloop.avg_duration = time.mainloop.avg_duration * time.mainloop.iter_counter + time.mainloop.duration;
            time.mainloop.avg_duration /= ++time.mainloop.iter_counter;

            //begin frame to signal everyone interested
            dispatcher.trigger<Events::Begin<Frame>>();

            //execute command buffers for this frame
            for (auto &command: *double_buffer.current) {
                command->execute();
            }

            double_buffer.current->clear();
            double_buffer.current->swap(*double_buffer.next);

            //begin frame to signal everyone interested
            dispatcher.trigger<Events::End<Frame>>();

            System::Window::Glfw::swap_and_poll_events();
        }

        double_buffer.current->clear();
        double_buffer.next->clear();

        //shutdown engine
        Log::Info("Engine: Shutdown...");
        dispatcher.trigger<Events::Shutdown<Engine>>();
        time.engine_run_end = Time::Point::Now();
    }
}