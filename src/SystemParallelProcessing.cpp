//
// Created by alex on 29.11.23.
//

#include "SystemParallelProcessing.h"
#include "Engine.h"
#include "Events.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include "Components.h"
#include "Commands.h"
#include "imgui.h"

namespace Bcg {
    namespace SystemParallelProcessingInternal {
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

        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemParallelProcessingInternal::on_render_gui>();
                return;
            }

            if (ImGui::Begin("Parallel", &show_gui)) {
                auto &worker_pool = Engine::Context().get<WorkerPool>();
                ImGui::Text("Workers: %zu", worker_pool.workers.size());
                ImGui::Text("Tasks: %zu", worker_pool.tasks.size());
                static int num_threads = std::thread::hardware_concurrency() - 1;
                ImGui::InputInt("Threads", &num_threads);
                if (ImGui::Button(worker_pool.stop ? "Start" : "Stop")) {
                    if (!worker_pool.stop) {
                        SystemParallelProcessing().stop();
                    } else {
                        SystemParallelProcessing().start(num_threads);
                    }
                }
                if (ImGui::Button("Resize")) {
                    SystemParallelProcessing().resize(num_threads);
                }

                static int num_tests = 100;
                ImGui::InputInt("Tests", &num_tests);
                if (ImGui::Button("Test")) {
                    ParallelCommands command("TestParallel");
                    for (int i = 0; i < num_tests; ++i) {
                        command.add_command_sptr(std::make_shared<TaskCommand>("Test", [&]() {
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Parallel", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemParallelProcessingInternal::on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemParallelProcessingInternal::on_render_gui_menu>();

            SystemParallelProcessing::start(std::thread::hardware_concurrency() - 1);
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            SystemParallelProcessing().stop();
        }
    }

    SystemParallelProcessing::SystemParallelProcessing() : System("SystemParallelProcessing") {

    }

    void SystemParallelProcessing::pre_init() {
        Engine::Context().emplace<WorkerPool>();
    }

    void SystemParallelProcessing::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemParallelProcessingInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemParallelProcessingInternal::on_shutdown_engine>();

        Log::Info(m_name + ": Initialized").enqueue();
    }

    void SystemParallelProcessing::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemParallelProcessingInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemParallelProcessingInternal::on_shutdown_engine>();

        Log::Info(m_name + ": Removed").enqueue();
    }


    void SystemParallelProcessing::start(int num_threads) {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        worker_pool.stop = false;
        worker_pool.workers.clear();
        for (size_t i = 0; i < num_threads; ++i) {
            worker_pool.workers.emplace_back(&SystemParallelProcessingInternal::worker_thread);
        }
        Log::Info("SystemParallelProcessing: Started with " + std::to_string(num_threads) + " threads").enqueue();
    }

    void SystemParallelProcessing::stop() {
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

    void SystemParallelProcessing::resize(int num_threads) {
        stop();
        start(num_threads);
    }

    void SystemParallelProcessing::enqueue(std::shared_ptr<Command> command) {
        auto &worker_pool = Engine::Context().get<WorkerPool>();
        {
            std::unique_lock<std::mutex> lock(worker_pool.queueMutex);
            worker_pool.tasks.emplace(std::move(command));
        }
        worker_pool.condition.notify_one();
    }

    void SystemParallelProcessing::enqueue(std::vector<std::shared_ptr<Command>> commands) {
        if (commands.size() == 1) {
            enqueue(commands[0]);
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
}