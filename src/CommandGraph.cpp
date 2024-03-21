//
// Created by alex on 21.03.24.
//

#include "CommandGraph.h"
#include <queue>
#include <memory>

namespace Bcg {
    std::shared_ptr<CommandNode> CommandGraph::add_command(const std::shared_ptr<AbstractCommand> &command) {
        auto node = std::make_shared<CommandNode>(command);
        nodes.push_back(node);
        return node;
    }

    void CommandGraph::add_dependency(const std::shared_ptr<CommandNode> &node,
                                      const std::shared_ptr<CommandNode> &dependency) {
        node->dependencies.push_back(dependency);
        dependency->dependents.push_back(node);
    }

    void CommandGraph::remove_dependency(const std::shared_ptr<CommandNode> &node,
                                         const std::shared_ptr<CommandNode> &dependency) {
        cleanup_connections(node, dependency);
    }

    void CommandGraph::remove_command(std::shared_ptr<AbstractCommand> command) {
        nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [&](const std::shared_ptr<CommandNode> &node) {
            if (node->command == command) {
                // Cleanup for both directions
                for (auto &dep: node->dependencies) {
                    cleanup_connections(node, dep.lock());
                }
                for (auto &dep: node->dependents) {
                    cleanup_connections(dep.lock(), node);
                }
                return true;
            }
            return false;
        }), nodes.end());
    }

    std::vector<std::shared_ptr<CommandNode>> CommandGraph::topological_sort() {
        std::vector<std::shared_ptr<CommandNode>> sortedNodes;
        std::unordered_map<std::shared_ptr<CommandNode>, size_t> inDegree;
        std::queue<std::shared_ptr<CommandNode>> queue;

        // Initialize in-degree for each node
        for (const auto &node: nodes) {
            inDegree[node] = 0;
        }

        // Calculate in-degree (number of incoming edges) for each node
        for (const auto &node: nodes) {
            for (const auto &dep: node->dependencies) {
                inDegree[dep.lock()]++;
            }
        }

        // Enqueue all nodes with in-degree 0
        for (const auto &node: nodes) {
            if (inDegree[node] == 0) {
                queue.push(node);
            }
        }

        while (!queue.empty()) {
            auto u = queue.front();
            queue.pop();
            sortedNodes.push_back(u);

            for (const auto &dep: u->dependents) {
                auto depNode = dep.lock();
                if (--inDegree[depNode] == 0) {
                    queue.push(depNode);
                }
            }
        }

        // Verify if topological sort is possible (DAG)
        if (sortedNodes.size() != nodes.size()) {
            throw std::runtime_error("Cycle detected in CommandGraph, topological sort not possible.");
        }

        nodes = sortedNodes;
        return sortedNodes;
    }

    std::pair<std::unordered_map<std::shared_ptr<CommandNode>, int>,
            std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>>>
    CommandGraph::assign_and_group_levels(const std::vector<std::shared_ptr<CommandNode>> &sortedNodes) {
        std::unordered_map<std::shared_ptr<CommandNode>, int> levels;
        std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>> levelGroups;

        for (const auto &node: sortedNodes) {
            int level = 0;
            for (const auto &dep: node->dependencies) {
                int depLevel = levels[dep.lock()] + 1;
                level = std::max(level, depLevel);
            }
            levels[node] = level;
            levelGroups[level].push_back(node);
        }
        return {levels, levelGroups};
    }

    void CommandGraph::execute_levels(
            const std::unordered_map<int, std::vector<std::shared_ptr<CommandNode>>> &levelGroups) {
        for (const auto &[level, nodes]: levelGroups) {
            if (nodes.size() > 1) {
                auto parallelCmds = std::make_shared<ParallelCommands>("Parallel Level " + std::to_string(level));
                for (const auto &node: nodes) {
                    parallelCmds->add_command_sptr(
                            node->command); // Assuming addCommand exists and works as described
                }
                parallelCmds->execute();
            } else if (!nodes.empty()) {
                nodes.front()->command->execute();
            }
        }
    }

    void CommandGraph::execute() {
        // Perform topological sort and level assignment
        topological_sort();
        auto [levels, groups] = assign_and_group_levels(nodes);
        // Group nodes by levels
        // Execute each level, utilizing ParallelCommands for parallel execution within a level
        execute_levels(groups);
    }

    void CommandGraph::cleanup_connections(const std::shared_ptr<CommandNode> &node,
                                           const std::shared_ptr<CommandNode> &target) {
        if (!node || !target) return;

        // Removing target from node's dependencies
        node->dependencies.erase(std::remove_if(node->dependencies.begin(), node->dependencies.end(),
                                                [&target](const std::weak_ptr<CommandNode> &dep) {
                                                    return dep.lock() == target;
                                                }), node->dependencies.end());

        // Removing node from target's dependents
        target->dependents.erase(std::remove_if(target->dependents.begin(), target->dependents.end(),
                                                [&node](const std::weak_ptr<CommandNode> &dep) {
                                                    return dep.lock() == node;
                                                }), target->dependents.end());
    }
}