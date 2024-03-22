//
// Created by alex on 21.03.24.
//

#ifndef ENGINE23_COMMANDGRAPH_H
#define ENGINE23_COMMANDGRAPH_H

#include <utility>
#include <mutex>

#include "Commands.h"

namespace Bcg {
    class CommandNode {
    public:
        std::shared_ptr<AbstractCommand> command;
        std::vector<std::weak_ptr<CommandNode>> dependencies;
        std::vector<std::weak_ptr<CommandNode>> dependents;
        std::mutex node_mutex;

        explicit CommandNode(std::shared_ptr<AbstractCommand> command) : command(std::move(command)) {}
    };

    class CommandGraph {
    public:
        std::vector<std::shared_ptr<CommandNode>> nodes;
        std::mutex nodes_mutex;

        CommandGraph() = default;

        bool contains(const std::shared_ptr<CommandNode> &node);

        std::shared_ptr<CommandNode> add_command(const std::shared_ptr<AbstractCommand> &command);

        static void
        add_dependency(const std::shared_ptr<CommandNode> &node, const std::shared_ptr<CommandNode> &dependency);

        static void
        remove_dependency(const std::shared_ptr<CommandNode> &node, const std::shared_ptr<CommandNode> &dependency);

        void remove_command(std::shared_ptr<AbstractCommand> command);

        std::vector<std::shared_ptr<CommandNode>> topological_sort(bool *graph_has_cycle = nullptr);

        static std::pair<std::unordered_map<std::shared_ptr<CommandNode>, int>,
                std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>>>
        assign_and_group_levels(const std::vector<std::shared_ptr<CommandNode>> &sortedNodes);

        static void
        execute_levels(const std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>> &levelGroups);

        void execute();
    };
}
#endif //ENGINE23_COMMANDGRAPH_H
