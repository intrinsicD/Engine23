#include "CommandGraph.h"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>

namespace Bcg {

// Mock Command to use in tests
    class MockCommand : public AbstractCommand {
    public:
        explicit MockCommand(const std::string &name = "MockCommand") : AbstractCommand(name) {}

        ~MockCommand() override = default;

        int execute() override {
            // Mock execution, simply return a success code
            return 0;
        }
    };

    TEST(CommandGraphAdjTest, AddCommand) {
        CommandGraphAdj graph;
        auto command = std::make_shared<MockCommand>();
        auto node_id = graph.add_command(command);

        // Check that the command was added correctly
        ASSERT_NE(node_id, -1); // Node should not be null
        EXPECT_EQ(graph.commands[node_id], command); // The node's command should be the one we added
    }

    TEST(CommandGraphAdjTest, AddDependency) {
        CommandGraphAdj graph;
        auto command1 = std::make_shared<MockCommand>();
        auto command2 = std::make_shared<MockCommand>();

        auto node1 = graph.add_command(command1);
        auto node2 = graph.add_command(command2);

        graph.add_dependency(node1, node2);

        // Check that the dependency was added correctly
        ASSERT_FALSE(graph.dependencies[node1].empty()); // Node1 should have a dependency
        EXPECT_EQ(graph.dependencies[node1].front(), node2); // The dependency should be node2
        EXPECT_EQ(graph.dependents[node2].front(), node1); // Node2 should list node1 as a dependent
    }


    TEST(CommandGraphAdjTest, TopologicalSort) {
        // This test would create a small graph with known dependencies, perform topological_sort,
        // and then verify the order of nodes matches the expected topological order.
        // Implementing this requires a specific setup and might depend on how your graph is structured.

        CommandGraphAdj graph;
        auto nodeA = graph.add_command(std::make_shared<MockCommand>("A"));
        auto nodeB = graph.add_command(std::make_shared<MockCommand>("B"));
        auto nodeC = graph.add_command(std::make_shared<MockCommand>("C"));
        auto nodeD = graph.add_command(std::make_shared<MockCommand>("D"));

        // Setting up a graph with edges A -> D, B -> D, C -> B
        graph.add_dependency(nodeA, nodeD);
        graph.add_dependency(nodeB, nodeD);
        graph.add_dependency(nodeC, nodeB);

        // The expected topological order is A, C, B, D or C, A, B, D (since A and C are independent)

        bool graph_has_cycle = false;
        auto sortedNodes = graph.topological_sort(&graph_has_cycle);

        EXPECT_FALSE(graph_has_cycle);

        // Convert sorted nodes to their IDs for easier comparison
        std::vector<std::string> sortedNodeIds;
        for (auto node_id: sortedNodes) {
            sortedNodeIds.push_back(graph.commands[node_id]->name);
        }

        // Verify the order
        // Note: This assertion assumes one of the possible correct orders for simplicity.
        // You may need to adjust it based on your specific graph structure and expected outcomes.
        std::vector<std::vector<std::string>> expectedOrders = {
                {"D", "A", "B", "C"},
                {"D", "B", "A", "C"},
                {"D", "B", "C", "A"}
        };
        bool correctOrder =
                std::find(expectedOrders.begin(), expectedOrders.end(), sortedNodeIds) != expectedOrders.end();


        EXPECT_TRUE(correctOrder);
    }

    TEST(CommandGraphAdjTest, TopologicalSortCycle) {
        // This test would create a small graph with known dependencies, perform topological_sort,
        // and then verify the order of nodes matches the expected topological order.
        // Implementing this requires a specific setup and might depend on how your graph is structured.

        CommandGraphAdj graph;
        auto nodeA = graph.add_command(std::make_shared<MockCommand>("A"));
        auto nodeB = graph.add_command(std::make_shared<MockCommand>("B"));
        auto nodeC = graph.add_command(std::make_shared<MockCommand>("C"));
        auto nodeD = graph.add_command(std::make_shared<MockCommand>("D"));

        // Setting up a graph with edges A -> D, B -> D, C -> B
        graph.add_dependency(nodeA, nodeD);
        graph.add_dependency(nodeB, nodeD);
        graph.add_dependency(nodeC, nodeB);
        graph.add_dependency(nodeD, nodeC);

        // The is a cycle in the graph which should be detected

        bool graph_has_cycle = false;
        auto sortedNodes = graph.topological_sort(&graph_has_cycle);

        EXPECT_TRUE(graph_has_cycle);
    }


    TEST(CommandGraphAdjPerformanceTest, TopologicalSortPerformance) {
        CommandGraphAdj graph;
        const size_t numCommands = 100000; // Adjust based on your performance testing needs

        // Create and add a large number of commands
        for (size_t i = 0; i < numCommands; ++i) {
            auto command = std::make_shared<MockCommand>(std::to_string(i));
            graph.add_command(command);
        }

        // Create dependencies - for simplicity, each command depends on the previous one
        // This setup creates a simple linear dependency chain. Adjust as needed for more complex scenarios
        auto prevNode = graph.add_command(std::make_shared<MockCommand>());
        for (size_t i = 1; i < numCommands; ++i) {
            auto node = graph.add_command(std::make_shared<MockCommand>());
            graph.add_dependency(prevNode, node);
            prevNode = node;
        }

        // Benchmark the topological sort
        auto start = std::chrono::high_resolution_clock::now();
        graph.topological_sort();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "CommandGraphAdj: Topological sort of " << numCommands << " commands took " << duration << " ms\n";

        // Optionally, assert on duration if you have a performance requirement
        // EXPECT_LT(duration, expectedMaxDuration);
    }
} // namespace Bcg