//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "Factories.h"
#include "Platform.h"

#include <iostream>

namespace Bcg{
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
        state.ctx().emplace<StartupCommand>();
        state.ctx().emplace<ShutdownCommand>();
        state.ctx().emplace<Platform>();

        SystemFactory::create_or_get_window_system();
        SystemFactory::create_or_get_render_system();
        SystemFactory::create_or_get_physics_system();
        SystemFactory::create_or_get_animation_system();
        SystemFactory::create_or_get_audio_system();
        SystemFactory::create_or_get_input_system();
        SystemFactory::create_or_get_scripting_system();
        SystemFactory::create_or_get_network_system();
        SystemFactory::create_or_get_gui_system();
        SystemFactory::create_or_get_plugin_system();

        ManagerFactory::create_or_get_time_manager();
        ManagerFactory::create_or_get_command_buffer_manager();
        ManagerFactory::create_or_get_worker_pool_manager();
        ManagerFactory::create_or_get_window_manager();
    }

    Engine::~Engine() {
        entt::locator<Engine *>::reset();
    }

    void Engine::run() {
        is_running = true;
        //startup engine
        Context().get<StartupCommand>().execute();
        auto *time_manager = ManagerFactory::create_or_get_time_manager();
        auto *command_buffer_manager = ManagerFactory::create_or_get_command_buffer_manager();
        ManagerFactory::create_or_get_worker_pool_manager();
        auto *window_manager = ManagerFactory::create_or_get_window_manager();
        window_manager->create_window(800, 600, "Engine23");

        ParallelCommands parallel_commands("Engine Mainloop");
        TaskCommand task_a("Task A", []() {
            std::cout << "Task A\n";
            return 1;
        });
        TaskCommand task_b("Task B", []() {
            std::cout << "Task B\n";
            return 1;
        });
        TaskCommand task_c("Task C", []() {
            std::cout << "Task C\n";
            return 1;
        });
        TaskCommand task_d("Task D", []() {
            std::cout << "Task D\n";
            return 1;
        });
        parallel_commands.add_command_sptr(std::make_shared<TaskCommand>(task_a));
        parallel_commands.add_command_sptr(std::make_shared<TaskCommand>(task_b));
        parallel_commands.add_command_sptr(std::make_shared<TaskCommand>(task_c));
        parallel_commands.add_command_sptr(std::make_shared<TaskCommand>(task_d));

        command_buffer_manager->push_command_to_process_user_commands(
                std::make_shared<ParallelCommands>(parallel_commands));
        while (is_running) {
            time_manager->begin_frame();
            window_manager->begin_frame();
            command_buffer_manager->update();
            window_manager->end_frame();
            time_manager->end_frame();
/*            is_running = false;*/
        }
        //shutdown engine
        Context().get<ShutdownCommand>().execute();
    }
}