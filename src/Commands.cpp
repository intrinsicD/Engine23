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
        return CompositeCommand::execute();
    }

    //------------------------------------------------------------------------------------------------------------------

    RenderCommand::RenderCommand() : CompositeCommand("Render") {

    }

    //------------------------------------------------------------------------------------------------------------------

    CleanupCommand::CleanupCommand() : CompositeCommand("Cleanup") {

    }

    //------------------------------------------------------------------------------------------------------------------

    namespace Log {
        struct Message : public Command {
            explicit Message(std::string type, std::string color, std::string message, double time_stamp) : Command(
                    "[" + type + "] " + color + message + "\033[0m" + " [" + std::to_string(time_stamp) + "] ") {}

            ~Message() override = default;

            int execute() override {
                std::cout << name << std::endl;
                return 1;
            }
        };

        Info::Info(std::string message) : Command("Info"), message(message), time_stamp(
                Time::Point::Now().duration<Time::Unit::seconds>(Engine::State().ctx().get<Time>().engine_constructor_start)) {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Info) return;
            Engine::State().ctx().get<CommandBufferNext>().emplace_back(
                    std::make_shared<Message>(name, "\033[1;32m", message, time_stamp));
        }

        int Info::execute() {
            return Message(name, "\033[1;32m", message, time_stamp).execute();
        }

        Warn::Warn(std::string message) : Command("Warn"), message(message), time_stamp(
                Time::Point::Now().duration<Time::Unit::seconds>(Engine::State().ctx().get<Time>().engine_constructor_start)) {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Warn) return;
            Engine::State().ctx().get<CommandBufferNext>().emplace_back(
                    std::make_shared<Message>(name, "\033[1;33m", message, time_stamp));
        }

        int Warn::execute() {
            return Message(name, "\033[1;33m", message, time_stamp).execute();
        }

        Error::Error(std::string message) : Command("Error"), message(message), time_stamp(
                Time::Point::Now().duration<Time::Unit::seconds>(Engine::State().ctx().get<Time>().engine_constructor_start)) {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Error) return;
            Engine::State().ctx().get<CommandBufferNext>().emplace_back(
                    std::make_shared<Message>(name, "\033[1;31m", message, time_stamp));
        }

        int Error::execute() {
            return Message(name, "\033[1;31m", message, time_stamp).execute();
        }

        TODO::TODO(std::string message) : Command("TODO"), message(message), time_stamp(
                Time::Point::Now().duration<Time::Unit::seconds>(Engine::State().ctx().get<Time>().engine_constructor_start)) {
            Engine::State().ctx().get<CommandBufferNext>().emplace_back(
                    std::make_shared<Message>("\033[1;31m" + name + "\033[0m", "\033[1;33m", message, time_stamp));
        }

        int TODO::execute() {
            return Message("\033[1;31m" + name + "\033[0m", "\033[1;33m", message, time_stamp).execute();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
}
