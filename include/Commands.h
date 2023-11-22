//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_COMMANDS_H
#define ENGINE23_COMMANDS_H

#include <string>
#include <memory>
#include <functional>

#include "Components.h"

namespace Bcg {
    struct Command {
        explicit Command(std::string name);

        virtual ~Command() = default;

        virtual int execute() = 0;

        [[nodiscard]] virtual size_t num_commands() const;

        [[nodiscard]] virtual std::string to_string() const;

        std::string name;
    };

    struct CompositeCommand : public Command {
        explicit CompositeCommand(std::string name);

        ~CompositeCommand() override = default;

        CompositeCommand &add_command_sptr(std::shared_ptr<Command> command);

        int execute() override;

        [[nodiscard]] size_t num_commands() const override;

    protected:
        CommandBuffer commands;
    };

    struct TaskCommand : public Command {
        TaskCommand(std::string name, std::function<int()> task);

        ~TaskCommand() override = default;

        int execute() override;

        std::function<int()> task;
    };

    struct ParallelCommands : public CompositeCommand {
        explicit ParallelCommands(std::string name);

        ~ParallelCommands() override = default;

        int execute() override;
    };

    struct StartupCommand : public CompositeCommand {
        StartupCommand();

        ~StartupCommand() override = default;
    };

    struct ShutdownCommand : public CompositeCommand {
        ShutdownCommand();

        ~ShutdownCommand() override = default;
    };

    struct ProcessUserCommands : public CompositeCommand {
        ProcessUserCommands();

        ~ProcessUserCommands() override = default;
    };

    struct UpdateStateCommand : public CompositeCommand {
        UpdateStateCommand();

        ~UpdateStateCommand() override = default;

        int execute() override;
    };

    struct RenderCommand : public CompositeCommand {
        RenderCommand();

        ~RenderCommand() override = default;
    };

    struct CleanupCommand : public CompositeCommand {
        CleanupCommand();

        ~CleanupCommand() override = default;
    };
}

#endif //ENGINE23_COMMANDS_H
