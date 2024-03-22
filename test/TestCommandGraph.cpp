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

    TEST(CommandGraphTest, AddCommand) {
        CommandGraph graph;
        auto command = std::make_shared<MockCommand>();
        auto node = graph.add_command(command);

        // Check that the command was added correctly
        ASSERT_NE(node, nullptr); // Node should not be null
        EXPECT_EQ(node->command, command); // The node's command should be the one we added
    }

    TEST(CommandGraphTest, AddDependency) {
        CommandGraph graph;
        auto command1 = std::make_shared<MockCommand>();
        auto command2 = std::make_shared<MockCommand>();

        auto node1 = graph.add_command(command1);
        auto node2 = graph.add_command(command2);

        graph.add_dependency(node1, node2);

        // Check that the dependency was added correctly
        ASSERT_FALSE(node1->dependencies.empty()); // Node1 should have a dependency
        EXPECT_EQ(node1->dependencies.front().lock(), node2); // The dependency should be node2
        EXPECT_EQ(node2->dependents.front().lock(), node1); // Node2 should list node1 as a dependent
    }

    TEST(CommandGraphTest, RemoveDependency) {
        CommandGraph graph;
        auto command1 = std::make_shared<MockCommand>();
        auto command2 = std::make_shared<MockCommand>();

        auto node1 = graph.add_command(command1);
        auto node2 = graph.add_command(command2);

        graph.add_dependency(node1, node2);
        graph.remove_dependency(node1, node2);

        // Check that the dependency was removed correctly
        EXPECT_TRUE(node1->dependencies.empty()); // Node1's dependencies should be empty
        EXPECT_TRUE(node2->dependents.empty()); // Node2's dependents should be empty
    }

    TEST(CommandGraphTest, RemoveCommand) {
        CommandGraph graph;
        auto command = std::make_shared<MockCommand>();
        auto node = graph.add_command(command);

        graph.remove_command(command);

        // Assuming CommandGraph provides a way to check if a node exists
        // For this test, you might need to add functionality to CommandGraph or adjust the test logic
        EXPECT_FALSE(graph.contains(node)); // The node should no longer exist in the graph
    }

    TEST(CommandGraphTest, TopologicalSort) {
        // This test would create a small graph with known dependencies, perform topological_sort,
        // and then verify the order of nodes matches the expected topological order.
        // Implementing this requires a specific setup and might depend on how your graph is structured.

        CommandGraph graph;
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
        for (const auto &node: sortedNodes) {
            auto command = std::dynamic_pointer_cast<MockCommand>(node->command);
            if (command) {
                sortedNodeIds.push_back(command->name);
            }
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

    TEST(CommandGraphTest, TopologicalSortCycle) {
        // This test would create a small graph with known dependencies, perform topological_sort,
        // and then verify the order of nodes matches the expected topological order.
        // Implementing this requires a specific setup and might depend on how your graph is structured.

        CommandGraph graph;
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


    TEST(CommandGraphPerformanceTest, TopologicalSortPerformance) {
        CommandGraph graph;
        const size_t numCommands = 10000; // Adjust based on your performance testing needs

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
        std::cout << "Topological sort of " << numCommands << " commands took " << duration << " ms\n";

        // Optionally, assert on duration if you have a performance requirement
        // EXPECT_LT(duration, expectedMaxDuration);
    }
} // namespace Bcg