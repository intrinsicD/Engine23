//
// Created by alex on 22.11.23.
//

#include "Commands.h"
#include "Engine.h"
#include "systems/SystemParallelProcessing.h"
#include "fmt/core.h"
#include "CommandDoubleBuffer.h"
#include "Time.h"

#include <iostream>
#include <memory>


namespace Bcg {
    AbstractCommand::AbstractCommand(std::string name) : name(std::move(name)) {}

    [[nodiscard]] size_t AbstractCommand::num_commands() const {
        return 1;
    }

    //------------------------------------------------------------------------------------------------------------------

    CompositeCommand::CompositeCommand(std::string name) : AbstractCommand(std::move(name)) {}



    CompositeCommand &CompositeCommand::add_task(std::string name, std::function<int()> task) {
        return add_command_sptr(std::make_shared<TaskCommand>(name, std::move(task)));
    }

    CompositeCommand &CompositeCommand::add_command_sptr(std::shared_ptr<AbstractCommand> command) {
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

    TaskCommand::TaskCommand(std::string name, std::function<int()> task) : AbstractCommand(std::move(name)),
                                                                            task(std::move(task)) {}

    int TaskCommand::execute() {
        return task();
    }

    //------------------------------------------------------------------------------------------------------------------

    ParallelCommands::ParallelCommands(std::string name) : CompositeCommand(std::move(name)) {}

    int ParallelCommands::execute() {
        auto size = commands.size();
        SystemParallelProcessing().enqueue(commands);
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
        Message::Message(std::string type, std::string color, std::string message, double time_stamp) : AbstractCommand(
                fmt::format("[{}] [{}] {}{}\033[0m", time_stamp, type, color, message)) {}

        Message::Message(std::string name) : AbstractCommand(std::move(name)) {}

        void Message::enqueue() {
            auto &double_buffer = Engine::Context().get<CommandDoubleBuffer>();
            double_buffer.enqueue_next(std::make_shared<Message>(name));
        }

        int Message::execute() {
            std::cout << name << std::endl;
            return 1;
        }

        Always::Always(std::string message) : Message("Always", "\033[1;32m", message,
                                                      Time::Point::Now().duration<Time::Unit::seconds>(
                                                              Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void Always::enqueue() {
            Message::enqueue();
        }

        Info::Info(std::string message) : Message("Info", "\033[1;32m", message,
                                                  Time::Point::Now().duration<Time::Unit::seconds>(
                                                          Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        Info::Info(std::string action, std::string message) : Message("Info", "\033[1;32m", action + ": " + message,
                                                                      Time::Point::Now().duration<Time::Unit::seconds>(
                                                                              Engine::State().ctx().get<Time>().engine_constructor_start)) {

        }

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

        TODO::TODO(std::string message) : Message("\033[1;31mTODO\033[0m", "\033[1;33m", message,
                                                  Time::Point::Now().duration<Time::Unit::seconds>(
                                                          Engine::State().ctx().get<Time>().engine_constructor_start)) {}

        void TODO::enqueue() {
            Message::enqueue();
        }

        SystemInit::SystemInit(std::string message) : Message("Info", "\033[1;32m", "Initialized: " + message,
                                                              Time::Point::Now().duration<Time::Unit::seconds>(
                                                                      Engine::State().ctx().get<Time>().engine_constructor_start)) {

        }

        void SystemInit::enqueue() {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Info) return;
            Message::enqueue();
        }

        SystemRemove::SystemRemove(std::string message) : Message("Info", "\033[1;32m", "Removed: " + message,
                                                              Time::Point::Now().duration<Time::Unit::seconds>(
                                                                      Engine::State().ctx().get<Time>().engine_constructor_start)) {

        }

        void SystemRemove::enqueue() {
            if (Engine::State().ctx().get<LogLevel>() < LogLevel::Info) return;
            Message::enqueue();
        }
    }

    //------------------------------------------------------------------------------------------------------------------
}
