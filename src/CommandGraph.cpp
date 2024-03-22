//
// Created by alex on 21.03.24.
//

#include "CommandGraph.h"
#include <queue>
#include <memory>

namespace Bcg {
    namespace CommandGraphInternal {
        inline void
        RemoveConnection(std::vector<std::weak_ptr<CommandNode>> &elements, const std::shared_ptr<CommandNode> &node) {
            elements.erase(std::remove_if(elements.begin(), elements.end(),
                                          [&node](const std::weak_ptr<CommandNode> &element) {
                                              return element.lock() == node;
                                          }), elements.end());
        }

        inline void CleanupConnectionsForNode(const std::shared_ptr<CommandNode> &node) {
            // Assuming all necessary locks are already acquired.

            // Remove node from its dependencies' dependents
            for (auto &weakDep: node->dependencies) {
                auto dep = weakDep.lock();
                if (dep) {
                    RemoveConnection(dep->dependents, node);
                }
            }

            // Remove node from its dependents' dependencies
            for (auto &weakDep: node->dependents) {
                auto dep = weakDep.lock();
                if (dep) {
                    RemoveConnection(dep->dependencies, node);
                }
            }
        }

        inline void CleanupConnections(const std::shared_ptr<CommandNode> &node,
                                       const std::shared_ptr<CommandNode> &target) {
            if (!node || !target) return;

            // Correctly removing target from node's dependencies
            RemoveConnection(node->dependencies, target);

            // Correctly removing node from target's dependents
            RemoveConnection(target->dependents, node);
        }
    }

    bool CommandGraph::contains(const std::shared_ptr<CommandNode> &node) {
        return std::find_if(nodes.begin(), nodes.end(), [&node](const std::shared_ptr<CommandNode> &n) {
            return n == node;
        }) != nodes.end();
    }

    std::shared_ptr<CommandNode> CommandGraph::add_command(const std::shared_ptr<AbstractCommand> &command) {
        std::lock_guard<std::mutex> lock(nodes_mutex);
        auto node = std::make_shared<CommandNode>(command);
        nodes.push_back(node);
        return node;
    }

    void CommandGraph::add_dependency(const std::shared_ptr<CommandNode> &node,
                                      const std::shared_ptr<CommandNode> &dependency) {
        std::scoped_lock lock(node->node_mutex, dependency->node_mutex);
        node->dependencies.push_back(dependency);
        dependency->dependents.push_back(node);
    }

    void CommandGraph::remove_dependency(const std::shared_ptr<CommandNode> &node,
                                         const std::shared_ptr<CommandNode> &dependency) {
        std::scoped_lock lock(node->node_mutex, dependency->node_mutex);
        CommandGraphInternal::CleanupConnections(node, dependency);
    }

    void CommandGraph::remove_command(std::shared_ptr<AbstractCommand> command) {
        // Step 1: Collect all nodes that need to be locked
        std::vector<std::shared_ptr<CommandNode>> nodesToLock;
        {
            std::lock_guard<std::mutex> lock(nodes_mutex);
            for (const auto &node: nodes) {
                if (node->command == command) {
                    nodesToLock.push_back(node);
                    for (const auto &dep_weak: node->dependencies) {
                        if (auto dep = dep_weak.lock()) {
                            nodesToLock.push_back(dep);
                        }
                    }
                    for (const auto &dep_weak: node->dependents) {
                        if (auto dep = dep_weak.lock()) {
                            nodesToLock.push_back(dep);
                        }
                    }
                }
            }
        }

        // Step 2: Sort nodes to lock by the address of their mutexes to avoid deadlock
        std::sort(nodesToLock.begin(), nodesToLock.end(),
                  [](const std::shared_ptr<CommandNode> &a, const std::shared_ptr<CommandNode> &b) {
                      return std::addressof(a->node_mutex) < std::addressof(b->node_mutex);
                  });

        // Step 3: Unique the vector to remove duplicates
        auto last = std::unique(nodesToLock.begin(), nodesToLock.end());
        nodesToLock.erase(last, nodesToLock.end());

        // Step 4: Lock all nodes' mutexes in order
        for (auto &node: nodesToLock) {
            node->node_mutex.lock();
        }

        // Step 5: Perform the actual removal now that we have exclusive access
        auto remove_it = std::remove_if(nodes.begin(), nodes.end(), [&](const std::shared_ptr<CommandNode> &node) {
            return node->command == command;
        });
        nodes.erase(remove_it, nodes.end());

        // Cleanup connections for all nodes, now that we've safely removed the command node
        // Assuming cleanup_connections is adapted to not lock internally since we've locked outside
        for (auto &node: nodesToLock) {
            CommandGraphInternal::CleanupConnectionsForNode(node);  // Implement this based on your logic
        }

        // Step 6: Unlock all mutexes
        for (auto &node: nodesToLock) {
            node->node_mutex.unlock();
        }
    }

    std::vector<std::shared_ptr<CommandNode>> CommandGraph::topological_sort(bool *graph_has_cycle) {
        std::lock_guard<std::mutex> lock(nodes_mutex);
        std::vector<std::shared_ptr<CommandNode>> sortedNodes;
        sortedNodes.reserve(nodes.size());
        std::unordered_map<std::shared_ptr<CommandNode>, size_t> inDegrees;
        inDegrees.reserve(nodes.size());
        std::queue<std::shared_ptr<CommandNode>> queue;

        // Initialize in-degree for each node
        for (const auto &node: nodes) {
            inDegrees[node] = 0;
        }

        // Calculate in-degree (number of incoming edges) for each node
        for (const auto &node: nodes) {
            for (const auto &dep_weak: node->dependencies) {
                auto dep = dep_weak.lock();
                if (dep) {
                    // Increment the in-degree of the node that depends on dep (the dependent), not dep itself.
                    inDegrees[node]++;
                }
            }
        }


        // Enqueue all nodes with in-degree 0
        for (const auto &[node, degree]: inDegrees) {
            if (degree == 0) {
                queue.push(node);
            }
        }

        while (!queue.empty()) {
            auto u = queue.front();
            queue.pop();
            sortedNodes.push_back(u);

            for (const auto &dep: u->dependents) {
                auto depNode = dep.lock();
                if (depNode && --inDegrees[depNode] == 0) {
                    queue.push(depNode);
                }
            }
        }

        // Verify if topological sort is possible (DAG)
        if (sortedNodes.size() != nodes.size()) {
            if (graph_has_cycle != nullptr) {
                *graph_has_cycle = true;
                return {};
            }
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
                    parallelCmds->add_command_sptr(node->command); // Assuming addCommand exists and works as described
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

    //------------------------------------------------------------------------------------------------------------------

    size_t CommandGraphAdj::contains(const std::shared_ptr<AbstractCommand> &command) {
        auto iter = std::find_if(commands.begin(), commands.end(), [&command](const auto &other) {
            return other == command;
        });
        if (iter != commands.end()) {
            return std::distance(commands.begin(), iter);
        }
        return -1;
    }

    size_t CommandGraphAdj::add_command(const std::shared_ptr<AbstractCommand> &command) {
        std::lock_guard<std::mutex> lock(graph_mutex);
        push_back();
        commands.back() = command;
        return commands.size() - 1;
    }

    void CommandGraphAdj::add_dependency(size_t node_id, size_t dependency_id) {
        std::lock_guard<std::mutex> lock(graph_mutex);
        dependencies[node_id].push_back(dependency_id);
        dependents[dependency_id].push_back(node_id);
    }

    std::vector<size_t> &CommandGraphAdj::topological_sort(bool *graph_has_cycle) {
        std::lock_guard<std::mutex> lock(graph_mutex);
        std::vector<size_t> inDegrees(commands.size(), 0);
        std::queue<size_t> queue;

        // Calculate in-degree (number of incoming edges) for each node
        for (size_t node_id = 0; node_id < commands.size(); ++node_id) {
            const auto &deps = dependencies[node_id];
            for (const auto &dep: deps) {
                inDegrees[node_id]++;
            }
        }

        // Enqueue all nodes with in-degree 0
        for (size_t node_id = 0; node_id < commands.size(); ++node_id) {
            if (inDegrees[node_id] == 0) {
                queue.push(node_id);
            }
        }

        ordering.resize(0);
        ordering.reserve(commands.size());
        while (!queue.empty()) {
            auto u = queue.front();
            queue.pop();
            ordering.push_back(u);

            const auto &deps = dependents[u];
            for (const auto &dep: deps) {
                if (--inDegrees[dep] == 0) {
                    queue.push(dep);
                }
            }
        }

        // Verify if topological sort is possible (DAG)
        if (ordering.size() != commands.size()) {
            if (graph_has_cycle != nullptr) {
                *graph_has_cycle = true;
            }
        }

        return ordering;
    }

    std::pair<std::unordered_map<size_t, int>, std::unordered_map<int, std::vector<size_t>>>
    CommandGraphAdj::assign_and_group_levels(const std::vector<size_t> &sortedNodes) {
        std::unordered_map<size_t, int> levels;
        std::unordered_map<int, std::vector<size_t>> levelGroups;

        for (const auto &node_id: sortedNodes) {
            int level = 0;
            const auto &deps = dependencies[node_id];
            for (const auto &dep: deps) {
                int depLevel = levels[dep] + 1;
                level = std::max(level, depLevel);
            }
            levels[node_id] = level;
            levelGroups[level].push_back(node_id);
        }
        return {levels, levelGroups};
    }

    void
    CommandGraphAdj::execute_levels(const std::unordered_map<int, std::vector<size_t>> &levelGroups) {
        for (const auto &[level, nodes]: levelGroups) {
            if (nodes.size() > 1) {
                auto parallelCmds = std::make_shared<ParallelCommands>("Parallel Level " + std::to_string(level));
                for (const auto &node_id: nodes) {
                    parallelCmds->add_command_sptr(commands[node_id]); // Assuming addCommand exists and works as described
                }
                parallelCmds->execute();
            } else if (!nodes.empty()) {
                commands[nodes.front()]->execute();
            }
        }
    }

    void CommandGraphAdj::execute() {
        // Perform topological sort and level assignment
        topological_sort();
        auto [levels, groups] = assign_and_group_levels(ordering);
        // Group nodes by levels
        // Execute each level, utilizing ParallelCommands for parallel execution within a level
        execute_levels(groups);
    }

    void CommandGraphAdj::push_back() {
        ordering.push_back(ordering.size());
        commands.emplace_back();
        dependencies.emplace_back();
        dependents.emplace_back();
    }
}