//
// Created by alex on 29.11.23.
//

#include "SystemParallelProcessing.h"
#include "Engine.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include "Components.h"
#include "Commands.h"
#include "imgui.h"

namespace Bcg::System::ParallelProcessing {

    void add_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&on_shutdown_engine>();
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
        Engine::Context().emplace<WorkerPool>();
        Log::Info("SystemParallelProcessing: Added").enqueue();
    }

    void remove_system() {
        Log::Info("SystemParallelProcessing: Removed").enqueue();
    }

    void worker_thread() {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        while (true) {
            std::shared_ptr<Command> task;
            {
                std::unique_lock<std::mutex> lock(worker_pool.queueMutex);
                worker_pool.condition.wait(lock, [&]() {
                    return worker_pool.stop || !worker_pool.tasks.empty();
                });
                if (worker_pool.stop && worker_pool.tasks.empty()) {
                    return;
                }
                task = std::move(worker_pool.tasks.front());
                worker_pool.tasks.pop();
            }
            task->execute();
        }
    }

    void start(int num_threads) {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        worker_pool.stop = false;
        worker_pool.workers.clear();
        for (size_t i = 0; i < num_threads; ++i) {
            worker_pool.workers.emplace_back(&worker_thread);
        }
        Log::Info("SystemParallelProcessing: Started with " + std::to_string(num_threads) + " threads").enqueue();
    }

    void stop() {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        {
            std::unique_lock<std::mutex> lock(worker_pool.queueMutex);
            worker_pool.stop = true;
        }
        worker_pool.condition.notify_all();
        for (auto &worker: worker_pool.workers) {
            worker.join();
        }
        Log::Info("SystemParallelProcessing: Stopped " + std::to_string(worker_pool.workers.size()) +
                  " threads").enqueue();
    }

    void resize(int num_threads) {
        stop();
        start(num_threads);
    }

    void enqueue_parallel(std::shared_ptr<Command> command) {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        {
            std::unique_lock<std::mutex> lock(worker_pool.queueMutex);
            worker_pool.tasks.emplace(std::move(command));
        }
        worker_pool.condition.notify_one();
    }

    void enqueue_parallel(std::vector<std::shared_ptr<Command>> commands) {
        if (commands.size() == 1) {
            enqueue_parallel(commands[0]);
            return;
        } else {
            auto &worker_pool = Engine::Context().get<WorkerPool>();
            {
                std::unique_lock<std::mutex> lock(worker_pool.queueMutex);
                for (auto &command: commands) {
                    worker_pool.tasks.emplace(std::move(command));
                }
            }
            worker_pool.condition.notify_all();
        }
    }

    void on_startup_engine(const Events::Startup<Engine> &event) {
        start(std::thread::hardware_concurrency() - 1);
    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
        stop();
    }

    static bool show_gui = false;

    void on_begin_frame(const Events::Begin<Frame> &event) {
        auto &double_buffer = Engine::State().ctx().get<CommandDoubleBuffer>();
        double_buffer.enqueue_next(std::make_shared<TaskCommand>("GuiMenu", [&]() {
            if (ImGui::BeginMenu("Menu")) {
                ImGui::MenuItem("Parallel", nullptr, &show_gui);
                ImGui::EndMenu();
            }
            return 1;
        }));

        if (show_gui) {
            double_buffer.enqueue_next(std::make_shared<TaskCommand>("GuiWindow", [&]() {
                if (ImGui::Begin("Parallel", &show_gui)) {
                    auto &worker_pool = Engine::Context().get<WorkerPool>();
                    ImGui::Text("Workers: %zu", worker_pool.workers.size());
                    ImGui::Text("Tasks: %zu", worker_pool.tasks.size());
                    static int num_threads = std::thread::hardware_concurrency() - 1;
                    ImGui::InputInt("Threads", &num_threads);
                    if (ImGui::Button(worker_pool.stop ? "Start" : "Stop")) {
                        if (!worker_pool.stop){
                            stop();
                        }else{
                            start(num_threads);
                        }
                    }
                    if(ImGui::Button("Resize")){
                        resize(num_threads);
                    }

                    static int num_tests = 10;
                    ImGui::InputInt("Tests", &num_tests);
                    if(ImGui::Button("Test")){
                        ParallelCommands command("TestParallel");
                        for(int i = 0; i < num_tests; ++i){
                            command.add_command_sptr(std::make_shared<TaskCommand>("Test", [&](){
                                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                                //thread id
                                auto id = std::this_thread::get_id();
                                std::stringstream ss;
                                ss << "Thread: " << id << " slept for 1 second\n";
                                Log::Info(ss.str()).enqueue();
                                return 1;
                            }));
                        }
                        command.execute();
                    }
                }
                ImGui::End();
                return 1;
            }));
        }
    }
}