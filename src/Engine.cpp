//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "Factories.h"
#include "Platform.h"
#include "GLFW/glfw3.h"

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


    }

    Engine::~Engine() {
        glfwInit();
        entt::locator<Engine *>::reset();
    }

    void Engine::run() {
        is_running = true;
        //The primary workflow is this:
        //1. notify all systems of an event
        //2. systems can add commands to the command buffer
        //3. execute the command buffer
        //4. repeat

        //initialize command buffers
        auto &command_buffers = state.ctx().emplace<CommandDoubleBuffers>();
        auto *command_buffers_current = &command_buffers.current;
        auto *command_buffers_next = &command_buffers.next;

        //startup engine
        dispatcher.trigger<Events::Startup<Engine>>();

        //execute command buffers for startup
        for (auto &command_buffer: *command_buffers_current) {
            for (auto &command: command_buffer) {
                command->execute();
            }
        }
        command_buffers_current->clear();
        std::swap(command_buffers_current, command_buffers_next);

        while (is_running) {
            //begin frame to signal everyone interested
            dispatcher.trigger<Events::Begin<Frame>>();

            //execute command buffers for this frame
            for (auto &command_buffer: *command_buffers_current) {
                for (auto &command: command_buffer) {
                    command->execute();
                }
            }
            //clear current command buffer and swap with next command buffer
            command_buffers_current->clear();

            //swap current command buffers with next command buffers
            std::swap(command_buffers_current, command_buffers_next);

            //begin frame to signal everyone interested
            dispatcher.trigger<Events::End<Frame>>();
        }

        command_buffers_current->clear();
        //shutdown engine
        dispatcher.trigger<Events::Shutdown<Engine>>();

        //execute command buffers for shutdown
        for (auto &command_buffer: *command_buffers_current) {
            for (auto &command: command_buffer) {
                command->execute();
            }
        }
    }
}