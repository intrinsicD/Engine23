//
// Created by alex on 22.11.23.
//

#include "Commands.h"
#include "Engine.h"
#include "Factories.h"

#include <iostream>

namespace Bcg {
    Command::Command(std::string name) : name(std::move(name)) {}


    [[nodiscard]] size_t Command::num_commands() const {
        return 1;
    }

    [[nodiscard]] std::string Command::to_string() const {
        return name;
    }

    //------------------------------------------------------------------------------------------------------------------

    CompositeCommand::CompositeCommand(std::string name) : Command(std::move(name)) {}


    CompositeCommand &CompositeCommand::add_command_sptr(std::shared_ptr<Command> command) {
        commands.emplace_back(std::move(command));
        return *this;
    }

    int CompositeCommand::execute() {
        int success_count = 0;
        for (auto &command: commands) {
            success_count += command->execute();
        }
        return success_count;
    }

    [[nodiscard]] size_t CompositeCommand::num_commands() const { return commands.size(); };

    //------------------------------------------------------------------------------------------------------------------

    TaskCommand::TaskCommand(std::string name, std::function<int()> task) : Command(std::move(name)),
                                                                            task(std::move(task)) {}

    int TaskCommand::execute() {
        return task();
    }

    //------------------------------------------------------------------------------------------------------------------

    ParallelCommands::ParallelCommands(std::string name) : CompositeCommand(std::move(name)) {}

    int ParallelCommands::execute() {
        auto *worker_pool_manager = ManagerFactory::create_or_get_worker_pool_manager();
        worker_pool_manager->push_tasks(commands);
        return commands.size();
    }

    //------------------------------------------------------------------------------------------------------------------

    StartupCommand::StartupCommand() : CompositeCommand("Engine Startup") {

    }

    //------------------------------------------------------------------------------------------------------------------

    ShutdownCommand::ShutdownCommand() : CompositeCommand("Engine Shutdown") {

    }

    //------------------------------------------------------------------------------------------------------------------

    ProcessUserCommands::ProcessUserCommands() : CompositeCommand("Process User Commands") {

    }

    //------------------------------------------------------------------------------------------------------------------

    UpdateStateCommand::UpdateStateCommand() : CompositeCommand("Update State") {

    }

    int UpdateStateCommand::execute() {
        auto &state = Engine::State();
        auto view = state.view<System *>();
        size_t count = 0;
        for (auto id: view) {
            auto &system = state.get<System *>(id);
            std::cout << count++ << " " << name << " " << system->name << "\n";
            system->update();
        }
        return CompositeCommand::execute();
    }

    //------------------------------------------------------------------------------------------------------------------

    RenderCommand::RenderCommand() : CompositeCommand("Render") {

    }

    //------------------------------------------------------------------------------------------------------------------

    CleanupCommand::CleanupCommand() : CompositeCommand("Cleanup") {

    }

    //------------------------------------------------------------------------------------------------------------------
}
