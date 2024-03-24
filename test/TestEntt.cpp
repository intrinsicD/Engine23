//
// Created by alex on 24.03.24.
//
#include "entt/entt.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>

namespace Bcg {
// Define some components
    struct Position {
        float x, y;
    };

    struct Velocity {
        float vx, vy;
    };

// Test fixture for EnTT performance tests
    class EnTTPerfTest : public ::testing::Test {
    protected:
        entt::registry registry;
        static constexpr int numEntities = 100000;

        void SetUp() override {
            // Setup code before each test...
        }

        void TearDown() override {
            // Cleanup code after each test...
        }
    };

// Measure time taken to add entities
    TEST_F(EnTTPerfTest, AddEntities) {
        auto start = std::chrono::high_resolution_clock::now();

        // Add many entities
        for (int i = 0; i < numEntities; ++i) {
            registry.create();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "Time taken to add " << numEntities << " entities: " << duration.count() << "ms\n";
    }

// Measure time taken to add and remove components
    TEST_F(EnTTPerfTest, AddRemoveComponents) {
        // First, add some entities
        std::vector<entt::entity> entities(numEntities);
        for (auto &entity: entities) {
            entity = registry.create();
        }

        auto startAdd = std::chrono::high_resolution_clock::now();

        // Add components to each entity
        for (auto entity: entities) {
            registry.emplace<Position>(entity, 1.0f, 1.0f);
            registry.emplace<Velocity>(entity, 1.0f, 1.0f);
        }

        auto endAdd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationAdd = endAdd - startAdd;
        std::cout << "Time taken to add components to " << numEntities << " entities: " << durationAdd.count()
                  << "ms\n";

        auto startRemove = std::chrono::high_resolution_clock::now();

        // Remove components from each entity
        for (auto entity: entities) {
            registry.remove<Position>(entity);
            registry.remove<Velocity>(entity);
        }

        auto endRemove = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationRemove = endRemove - startRemove;
        std::cout << "Time taken to remove components from " << numEntities << " entities: " << durationRemove.count()
                  << "ms\n";
    }
}