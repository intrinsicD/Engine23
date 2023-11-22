//
// Created by alex on 22.11.23.
//

#include "Managers.h"
#include "Factories.h"
#include "entt/entity/entity.hpp"

#include <dlfcn.h>

namespace Bcg {
    Manager::Manager(std::string name) : name(std::move(name)), id(entt::null) {}

    //------------------------------------------------------------------------------------------------------------------

    CommandBufferManager::CommandBufferManager() : Manager("Command Buffer") {}

    void CommandBufferManager::execute_current_buffer() const {
        if (counter) {
            size_t success_count_current_frame = 0;
            size_t num_commands_current_frame = 0;
            for (auto &command: *current_buffer) {
                num_commands_current_frame += command->num_commands();
                success_count_current_frame += command->execute();
            }
            counter->total_success_count += success_count_current_frame;
            counter->total_num_commands += num_commands_current_frame;
        } else {
            for (auto &command: *current_buffer) {
                command->execute();
            }
        }
    }

    void CommandBufferManager::swap_buffers() const {
        current_buffer->clear();
        current_buffer->swap(*next_buffer);
    }

    void CommandBufferManager::clear_and_prepare_next_buffer() {
        next_buffer->clear();
        assert(sptr_process);
        assert(sptr_update);
        assert(sptr_render);
        assert(sptr_cleanup);

        push_command_to_command_buffer(sptr_process);
        push_command_to_command_buffer(sptr_update);
        push_command_to_command_buffer(sptr_render);
        push_command_to_command_buffer(sptr_cleanup);
    }

    void CommandBufferManager::update() {
        swap_buffers();
        execute_current_buffer();
        clear_and_prepare_next_buffer();
    }

    void CommandBufferManager::enable_counter() {
        counter = get_state().try_get<CommandBufferSuccessCounter>(id);
    }

    void CommandBufferManager::disable_counter() {
        counter = nullptr;
    }

    void CommandBufferManager::toggle_counter() {
        if (counter) {
            disable_counter();
        } else {
            enable_counter();
        }
    }

    void CommandBufferManager::push_command_to_process_user_commands(std::shared_ptr<Command> command) const {
        sptr_process->add_command_sptr(std::move(command));
    }

    void CommandBufferManager::push_command_to_update_state(std::shared_ptr<Command> command) const {
        sptr_update->add_command_sptr(std::move(command));
    }

    void CommandBufferManager::push_command_to_render(std::shared_ptr<Command> command) const {
        sptr_render->add_command_sptr(std::move(command));
    }

    void CommandBufferManager::push_command_to_cleanup(std::shared_ptr<Command> &command) const {
        sptr_cleanup->add_command_sptr(std::move(command));
    }

    void CommandBufferManager::push_command_to_command_buffer(std::shared_ptr<Command> command) const {
        //TODO lock_guard ?
        next_buffer->emplace_back(std::move(command));
    }

    //------------------------------------------------------------------------------------------------------------------


    TimeManager::TimeManager() : Manager("Time") {}

    void TimeManager::start_loop() const {
        time->mainloop.started = Time::Point::Now();
    }

    void TimeManager::end_loop() const {
        time->mainloop.current = Time::Point::Now().duration<Time::Unit::seconds>(time->mainloop.started);
        time->mainloop.avg =
                time->mainloop.avg * static_cast<double>(time->mainloop.counter) + time->mainloop.current;
        time->mainloop.avg /= static_cast<double>(++time->mainloop.counter);
        time->mainloop.fps = static_cast<int>(1.0 / time->mainloop.current);
        time->mainloop.avg_fps = static_cast<int>(1.0 / time->mainloop.avg);
    }

    //------------------------------------------------------------------------------------------------------------------


    void WorkerPoolManager::push_task(std::shared_ptr<Command> &task) const {
        {
            std::unique_lock<std::mutex> lock(worker_pool->queueMutex);
            worker_pool->tasks.emplace(std::move(task));
        }
        worker_pool->condition.notify_one();
    }

    void WorkerPoolManager::push_tasks(std::vector<std::shared_ptr<Command>> tasks) const {
        if (!tasks.empty()) {
            if (tasks.size() == 1) {
                push_task(tasks.front());
                return;
            } else {
                {
                    std::unique_lock<std::mutex> lock(worker_pool->queueMutex);
                    for (auto &task: tasks) {
                        worker_pool->tasks.emplace(std::move(task));
                    }
                }
                worker_pool->condition.notify_all();
            }
        }
    }

    WorkerPoolManager::WorkerPoolManager() : Manager("WorkerPool") {}

    size_t WorkerPoolManager::max_num_threads() {
        return std::thread::hardware_concurrency();
    }

    void WorkerPoolManager::start(size_t num_threads) {
        worker_pool->stop = false;
        for (size_t i = 0; i < num_threads; ++i) {
            worker_pool->workers.emplace_back(&WorkerPoolManager::worker_thread,
                                              ManagerFactory::create_or_get_worker_pool_manager());
        }
    }

    void WorkerPoolManager::stop() const {
        {
            std::unique_lock<std::mutex> lock(worker_pool->queueMutex);
            worker_pool->stop = true;
        }
        worker_pool->condition.notify_all();
        for (auto &worker: worker_pool->workers) {
            worker.join();
        }
    }

    void WorkerPoolManager::resize(size_t num_threads) {
        stop();
        worker_pool->workers.resize(num_threads);
        start(num_threads);
    }

    void WorkerPoolManager::worker_thread() const {
        while (true) {
            std::shared_ptr<Command> task;
            {
                std::unique_lock<std::mutex> lock(worker_pool->queueMutex);
                worker_pool->condition.wait(lock, [&]() {
                    return worker_pool->stop || !worker_pool->tasks.empty();
                });
                if (worker_pool->stop && worker_pool->tasks.empty()) {
                    return;
                }
                task = std::move(worker_pool->tasks.front());
                worker_pool->tasks.pop();
            }
            task->execute();
        }
    }

    //------------------------------------------------------------------------------------------------------------------

    PluginManager::PluginManager() : Manager("Plugin") {}

    std::string PluginManager::extract_plugin_name(const std::string &filepath) {
        size_t start = filepath.find(plugin_prefix);
        if (start == std::string::npos) {
            return ""; // Prefix not found
        }
        start += plugin_prefix.length(); // Move past the prefix
        size_t end = filepath.rfind(plugin_ext);
        if (end == std::string::npos) {
            return ""; // ".so" not found
        }

        return filepath.substr(start, end - start);
    }

    Plugin *PluginManager::load(std::string filepath) {
        void *handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (handle) {
            dlerror();
            auto create_instance_name = create_instance_prefix + extract_plugin_name(filepath);
            auto create = (Plugin *(*)()) dlsym(handle, create_instance_name.c_str());
            if (create) {
                const char *dlsym_error = dlerror();
                if (!dlsym_error) {
                    return create();
                }
            }
        }
        dlclose(handle);
        return nullptr;
    }

    void PluginManager::unload(Plugin *plugin, void *handle) {
        dlclose(handle);
    }

    //------------------------------------------------------------------------------------------------------------------


    void WindowManager::create_window(int width, int height, std::string title) {
        if (!window) {
            window = &get_state().emplace<Window>(id, width, height, std::move(title));
        }
    }

    void WindowManager::destroy_window() {
        get_state().remove<Window>(id);
        window = nullptr;
    }

    void WindowManager::resize_window(int width, int height) const {
        window->width = width;
        window->height = height;
    }

    void WindowManager::set_window_title(std::string title) const {
        window->title = std::move(title);
    }

    double WindowManager::get_aspect_ratio() const {
        return static_cast<double>(window->width) / static_cast<double>(window->height);
    }

    WindowManager::WindowManager() : Manager("Window") {}


}