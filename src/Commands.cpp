//
// Created by alex on 22.11.23.
//

#include "Commands.h"
#include "Engine.h"
#include "Factories.h"

#include <iostream>
#include <memory>


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
        auto size = commands.size();
        System::ParallelProcessing::enqueue_parallel(commands);
        return size;
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
        Message::Message(std::string type, std::string color, std::string message, double time_stamp) : Command(
                "[" + type + "] " + color + message + "\033[0m" + " [" + std::to_string(time_stamp) + "] ") {}

        Message::Message(std::string name) : Command(std::move(name)) {}

        void Message::enqueue() {
            auto &double_buffer = Engine::Context().get<CommandDoubleBuffer>();
            double_buffer.enqueue_next(std::make_shared<Message>(name));
        }

        int Message::execute() {
            std::cout << name << std::endl;
            return 1;
        }

        Info::Info(std::string message) : Message("Info", "\033[1;32m", message,
                                                  Time::Point::Now().duration<Time::Unit::seconds>(
                                                          Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void Info::enqueue() {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Info) return;
            Message::enqueue();
        }

        Warn::Warn(std::string message) : Message("Warn", "\033[1;33m", message,
                                                  Time::Point::Now().duration<Time::Unit::seconds>(
                                                          Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void Warn::enqueue() {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Warn) return;
            Message::enqueue();
        }

        Error::Error(std::string message) : Message("Error", "\033[1;31m", message,
                                                    Time::Point::Now().duration<Time::Unit::seconds>(
                                                            Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void Error::enqueue() {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Error) return;
            Message::enqueue();
        }

        TODO::TODO(std::string message) : Message("\033[1;31m TODO \033[0m", "\033[1;33m", message,
                                                  Time::Point::Now().duration<Time::Unit::seconds>(
                                                          Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void TODO::enqueue() {
            Message::enqueue();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
}
