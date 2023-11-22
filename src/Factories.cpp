//
// Created by alex on 22.11.23.
//

#include "Factories.h"
#include "Components.h"

namespace Bcg {

    //------------------------------------------------------------------------------------------------------------------

    CommandBufferManager *ManagerFactory::create_or_get_command_buffer_manager() {
        auto *manager = get_state_ctx().find<CommandBufferManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            CommandBufferManager command_buffer_manager;
            auto &state = get_state();
            command_buffer_manager.id = state.create();
            command_buffer_manager.current_buffer = &state.emplace<CommandBufferCurrent>(
                    command_buffer_manager.id);
            command_buffer_manager.next_buffer = &state.emplace<CommandBufferNext>(
                    command_buffer_manager.id);
            auto &process = state.emplace<ProcessUserCommands>(command_buffer_manager.id);
            command_buffer_manager.sptr_process = std::make_shared<ProcessUserCommands>(process);
            auto &update = state.emplace<UpdateStateCommand>(command_buffer_manager.id);
            command_buffer_manager.sptr_update = std::make_shared<UpdateStateCommand>(update);
            auto &render = state.emplace<RenderCommand>(command_buffer_manager.id);
            command_buffer_manager.sptr_render = std::make_shared<RenderCommand>(render);
            auto &cleanup = state.emplace<CleanupCommand>(command_buffer_manager.id);
            command_buffer_manager.sptr_cleanup = std::make_shared<CleanupCommand>(cleanup);
            command_buffer_manager.counter = &state.emplace<CommandBufferSuccessCounter>(
                    command_buffer_manager.id);
            return &get_state_ctx().emplace<CommandBufferManager>(command_buffer_manager);
        } else {
            return manager;
        }
    }

    //------------------------------------------------------------------------------------------------------------------

    TimeManager *ManagerFactory::create_or_get_time_manager() {
        auto *manager = get_state_ctx().find<TimeManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            TimeManager time_manager;
            auto &state = get_state();
            time_manager.id = state.create();
            time_manager.time = &state.emplace<Time>(time_manager.id);
            time_manager.time->engine_started = Time::Point::Now();
            return &state.ctx().emplace<TimeManager>(time_manager);
        } else {
            return manager;
        }
    }

    WorkerPoolManager *ManagerFactory::create_or_get_worker_pool_manager() {
        auto *manager = get_state_ctx().find<WorkerPoolManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            WorkerPoolManager worker_pool_manager;
            auto &state = get_state();
            worker_pool_manager.id = state.create();
            worker_pool_manager.worker_pool = &state.emplace<WorkerPool>(worker_pool_manager.id);
            return &state.ctx().emplace<WorkerPoolManager>(worker_pool_manager);
        } else {
            if (manager->worker_pool->stop) {
                manager->start();
            }
            return manager;
        }
    }

    WindowManager *ManagerFactory::create_or_get_window_manager() {
        auto *manager = get_state_ctx().find<WindowManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            WindowManager window_manager;
            auto &state = get_state();
            window_manager.id = state.create();
            window_manager.window = &state.emplace<Window>(window_manager.id, 800, 600, "Viewer");
            return &state.ctx().emplace<WindowManager>(window_manager);
        } else {
            return manager;
        }
    }

    //------------------------------------------------------------------------------------------------------------------

    RenderSystem *SystemFactory::create_or_get_render_system() {
        auto *system = get_state_ctx().find<RenderSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &renderer = state.ctx().emplace<RenderSystem>();
            renderer.id = state.create();
            state.emplace<System *>(renderer.id, &renderer);
            return &renderer;
        } else {
            return system;
        }
    }

    PhysicsSystem *SystemFactory::create_or_get_physics_system() {
        auto *system = get_state_ctx().find<PhysicsSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &physics = state.ctx().emplace<PhysicsSystem>();
            physics.id = state.create();
            state.emplace<System *>(physics.id, &physics);
            return &physics;
        } else {
            return system;
        }
    }

    AnimationSystem *SystemFactory::create_or_get_animation_system() {
        auto *system = get_state_ctx().find<AnimationSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &animation = state.ctx().emplace<AnimationSystem>();
            animation.id = state.create();
            state.emplace<System *>(animation.id, &animation);
            return &animation;
        } else {
            return system;
        }
    }

    AudioSystem *SystemFactory::create_or_get_audio_system() {
        auto *system = get_state_ctx().find<AudioSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &audio = state.ctx().emplace<AudioSystem>();
            audio.id = state.create();
            state.emplace<System *>(audio.id, &audio);
            return &audio;
        } else {
            return system;
        }
    }

    InputSystem *SystemFactory::create_or_get_input_system() {
        auto *system = get_state_ctx().find<InputSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &input = state.ctx().emplace<InputSystem>();
            input.id = state.create();
            state.emplace<System *>(input.id, &input);
            return &input;
        } else {
            return system;
        }
    }

    ScriptingSystem *SystemFactory::create_or_get_scripting_system() {
        auto *system = get_state_ctx().find<ScriptingSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &scripting = state.ctx().emplace<ScriptingSystem>();
            scripting.id = state.create();
            state.emplace<System *>(scripting.id, &scripting);
            return &scripting;
        } else {
            return system;
        }
    }

    NetworkSystem *SystemFactory::create_or_get_network_system() {
        auto *system = get_state_ctx().find<NetworkSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &network = state.ctx().emplace<NetworkSystem>();
            network.id = state.create();
            state.emplace<System *>(network.id, &network);
            return &network;
        } else {
            return system;
        }
    }

    GuiSystem *SystemFactory::create_or_get_gui_system() {
        auto *system = get_state_ctx().find<GuiSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &gui = state.ctx().emplace<GuiSystem>();
            gui.id = state.create();
            state.emplace<System *>(gui.id, &gui);
            return &gui;
        } else {
            return system;
        }
    }

    PluginSystem *SystemFactory::create_or_get_plugin_system() {
        auto *system = get_state_ctx().find<PluginSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &plugin = state.ctx().emplace<PluginSystem>();
            plugin.id = state.create();
            state.emplace<System *>(plugin.id, &plugin);
            return &plugin;
        } else {
            return system;
        }
    }

    WindowSystem *SystemFactory::create_or_get_window_system() {
        auto *system = get_state_ctx().find<WindowSystem>();
        if (!system) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            auto &state = get_state();
            auto &window = state.ctx().emplace<WindowSystem>();
            window.id = state.create();
            state.emplace<System *>(window.id, &window);
            return &window;
        } else {
            return system;
        }
    }

}