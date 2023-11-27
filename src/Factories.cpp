//
// Created by alex on 22.11.23.
//

#include "Factories.h"

namespace Bcg {

    //------------------------------------------------------------------------------------------------------------------

    CommandBufferManager *ManagerFactory::create_or_get_command_buffer_manager() {
        auto *manager = Engine::Context().find<CommandBufferManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            CommandBufferManager command_buffer_manager;
            auto &state = Engine::State();
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
            return &Engine::Context().emplace<CommandBufferManager>(command_buffer_manager);
        } else {
            return manager;
        }
    }

    //------------------------------------------------------------------------------------------------------------------

    WorkerPoolManager *ManagerFactory::create_or_get_worker_pool_manager() {
        auto *manager = Engine::Context().find<WorkerPoolManager>();
        if (!manager) {
            static std::mutex creationMutex;
            std::lock_guard<std::mutex> lock(creationMutex);
            WorkerPoolManager worker_pool_manager;
            auto &state = Engine::State();
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
}