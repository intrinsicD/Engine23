//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_COMMANDS_H
#define ENGINE23_COMMANDS_H

#include <string>
#include <memory>
#include <functional>
#include <sstream>

#include "EngineFwd.h"

namespace Bcg {
    template<typename Result=void, typename... Args>
    struct TestCommand {
        explicit TestCommand(std::function<Result(Args... args)> task) : task(std::move(task)) {}

        Result operator()(Args... args) { return task(args...); }

        std::function<Result(Args... args)> task;
    };

    struct AbstractCommand {
        explicit AbstractCommand(std::string name);

        virtual ~AbstractCommand() = default;

        virtual int execute() = 0;

        [[nodiscard]] virtual size_t num_commands() const;

        std::string name;
    };

    struct TaskCommand : public AbstractCommand {
        TaskCommand(std::string name, std::function<int()> task);

        ~TaskCommand() override = default;

        int execute() override;

        std::function<int()> task;
    };

    struct CompositeCommand : public AbstractCommand {
        explicit CompositeCommand(std::string name);

        ~CompositeCommand() override = default;

        CompositeCommand &add_task(std::string name, std::function<int()> task);

        template<typename CommandType>
        CompositeCommand &add_command(CommandType &command) {
            return add_command_sptr(std::make_shared<CommandType>(command));
        }

        CompositeCommand &add_command_sptr(std::shared_ptr<AbstractCommand> command);

        int execute() override;

        [[nodiscard]] size_t num_commands() const override;

    protected:
        std::vector<std::shared_ptr<AbstractCommand>> commands;
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

    namespace Log {
        struct Message : public AbstractCommand {
            explicit Message(std::string type, std::string color, std::string message, double time_stamp);

            explicit Message(std::string name);

            ~Message() override = default;

            virtual void enqueue();

            int execute() override;
        };

        struct Always : public Message {
            explicit Always(std::string message);

            ~Always() override = default;

            void enqueue() override;
        };

        struct Info : public Message {
            explicit Info(std::string message);

            explicit Info(std::string action, std::string message);

            ~Info() override = default;

            void enqueue() override;
        };

        struct Warn : public Message {
            explicit Warn(std::string message);

            ~Warn() override = default;

            void enqueue() override;
        };

        struct Error : public Message {
            explicit Error(std::string message);

            ~Error() override = default;

            void enqueue() override;
        };

        struct TODO : public Message {
            explicit TODO(std::string message);

            ~TODO() override = default;

            void enqueue() override;
        };
    }
}

#endif //ENGINE23_COMMANDS_H
