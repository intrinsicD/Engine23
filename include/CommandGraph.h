//
// Created by alex on 21.03.24.
//

#ifndef ENGINE23_COMMANDGRAPH_H
#define ENGINE23_COMMANDGRAPH_H

#include <utility>

#include "Commands.h"

namespace Bcg {
    class CommandNode {
    public:
        std::shared_ptr<AbstractCommand> command;
        std::vector<std::weak_ptr<CommandNode>> dependencies;
        std::vector<std::weak_ptr<CommandNode>> dependents;

        explicit CommandNode(std::shared_ptr<AbstractCommand> command) : command(std::move(command)) {}
    };

    class CommandGraph {
    public:
        std::vector<std::shared_ptr<CommandNode>> nodes;

        CommandGraph() = default;

        std::shared_ptr<CommandNode> add_command(const std::shared_ptr<AbstractCommand>& command);

        static void add_dependency(const std::shared_ptr<CommandNode>& node, const std::shared_ptr<CommandNode>& dependency);

        static void remove_dependency(const std::shared_ptr<CommandNode>& node, const std::shared_ptr<CommandNode>& dependency);

        void remove_command(std::shared_ptr<AbstractCommand> command);

        std::vector<std::shared_ptr<CommandNode>> topological_sort();

        static std::pair<std::unordered_map<std::shared_ptr<CommandNode>, int>,
                std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>>>
        assign_and_group_levels(const std::vector<std::shared_ptr<CommandNode>> &sortedNodes);

        static void execute_levels(const std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>> &levelGroups);

        void execute();

    private:
        static void cleanup_connections(const std::shared_ptr<CommandNode> &node, const std::shared_ptr<CommandNode> &target);
    };
}
#endif //ENGINE23_COMMANDGRAPH_H
