//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_MANAGERS_H
#define ENGINE23_MANAGERS_H

#include <string>
#include "Components.h"
#include "Commands.h"
#include "Engine.h"
#include "entt/fwd.hpp"

namespace Bcg {
    class ManagerFactory;

    class Manager {
    public:
        //Is an Engine component, it lives inside the Engine's state (registry)
        //Usually Manages a specific set of entities and components
        //Usually Manages a Cache of entities and components or a Cache of Instances of types
        //Should be specialized
        //Should be as independent as possible from other components of the Engine
        //Has access to the Engine's state via Engine::get_instance()->state
        //Has access to the Engine's dispatcher via Engine::get_instance()->dispatcher
        //Exposes functions for performance critical stuff
        //has protected functions for commands and events

        explicit Manager(std::string name);

        virtual ~Manager() = default;

        std::string name;
        entt::entity id;
    };


    class CommandBufferManager : public Manager {
    public:
        ~CommandBufferManager() override = default;

        void execute_current_buffer() const;

        void swap_buffers() const;

        void clear_and_prepare_next_buffer();

        void update();

        void enable_counter();

        void disable_counter();

        void toggle_counter();

        void push_command_to_process_user_commands(std::shared_ptr<Command> command) const;

        void push_command_to_update_state(std::shared_ptr<Command> command) const;

        void push_command_to_render(std::shared_ptr<Command> command) const;

        void push_command_to_cleanup(std::shared_ptr<Command> &command) const;

        void push_command_to_command_buffer(std::shared_ptr<Command> command) const;

        void push_command_to_startup(std::shared_ptr<Command> command) const;

        void push_command_to_shutdown(std::shared_ptr<Command> command) const;

        CommandBufferCurrent *current_buffer = nullptr;
        CommandBufferNext *next_buffer = nullptr;
        CommandBufferSuccessCounter *counter = nullptr;
        std::shared_ptr<ProcessUserCommands> sptr_process;
        std::shared_ptr<UpdateStateCommand> sptr_update;
        std::shared_ptr<RenderCommand> sptr_render;
        std::shared_ptr<CleanupCommand> sptr_cleanup;
    protected:
        friend ManagerFactory;

        CommandBufferManager();
    };

    class TimeManager : public Manager {
    public:
        ~TimeManager() override = default;

        void begin_frame() const;

        void end_frame() const;

        Time *time = nullptr;
    protected:
        TimeManager();

        friend ManagerFactory;
    };

    class WorkerPoolManager : public Manager {
    public:
        ~WorkerPoolManager() override = default;

        void push_task(std::shared_ptr<Command> &task) const;

        void push_tasks(std::vector<std::shared_ptr<Command>> tasks) const;

        WorkerPool *worker_pool = nullptr;

    protected:
        friend ManagerFactory;

        WorkerPoolManager();

        static size_t max_num_threads();

        void start(size_t num_threads = max_num_threads());

        void stop() const;

        void resize(size_t num_threads);

        void worker_thread() const;
    };

    class Plugin;

    class PluginManager : public Manager {
    public:
        ~PluginManager() override = default;

        std::string extract_plugin_name(const std::string &filepath);

        Plugin *load(std::string filepath);

        void unload(Plugin *plugin, void *handle);

        std::string plugin_prefix = "libbcg_plugin_";
        std::string plugin_ext = ".so";
        std::string create_instance_prefix = "create_instance_";

    protected:
        PluginManager();

        friend ManagerFactory;
    };

    class WindowManager : public Manager {
    public:
        ~WindowManager() override = default;

        virtual void create_window(int width, int height, std::string title);

        virtual void destroy_window();

        virtual void resize_window(int width, int height) const;

        virtual void set_window_title(std::string title) const;

        virtual double get_aspect_ratio() const;

        virtual void begin_frame() const = 0;

        virtual void end_frame() const = 0;

        Window *window = nullptr;
    protected:
        WindowManager();

        friend ManagerFactory;
    };
}

#endif //ENGINE23_MANAGERS_H
