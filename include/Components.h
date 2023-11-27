//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_COMPONENTS_H
#define ENGINE23_COMPONENTS_H

#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include "entt/fwd.hpp"

namespace Bcg {
    struct StartupWindowConfig {
        std::string title = "Viewer";
        int width = 800;
        int height = 600;
    };
    struct WindowConfig {
        std::string title = "Viewer";
        int width = 800;
        int height = 600;
        double aspect_ratio = 4.0 / 3.0;
        float background_color[4] = {0.1f, 0.3f, 0.6f, 1.0f};
    };
    struct OpenGLConfig {
        int major_hint = 4;
        int minor_hint = 6;
        int major;
        int minor;
        bool core = true;
        bool forward_compatibility = true;
        bool debug = true;
        std::string vendor;
        std::string renderer;
        std::string version;
        std::string glsl_version;
    };
    struct Input {
        struct Mouse {
            struct Position {
                double x = 0;
                double y = 0;
            };
            struct Button {
                bool left = false;
                bool right = false;
                bool middle = false;
            };
            struct Scroll {
                double x = 0;
                double y = 0;
            };
            Position position;
            Scroll scroll;
            Button button;
        } mouse;
        struct Keyboard {
            std::vector<bool> keys;
        } keyboard;
    };

    struct Frame {
    };
    struct Command;
    struct CommandBuffer : public std::vector<std::shared_ptr<Command>> {
        using std::vector<std::shared_ptr<Command>>::vector;
    };

    struct CommandBufferCurrent : public CommandBuffer {
        using CommandBuffer::CommandBuffer;
    };

    struct CommandBufferNext : public CommandBuffer {
        using CommandBuffer::CommandBuffer;
    };

    struct CommandBufferSuccessCounter {
        size_t total_success_count = 0;
        size_t total_num_commands = 0;
    };

    struct Priority {
        int value = 0;
    };

    template<typename T>
    struct DurationTypeToString;

    struct Time {
        struct Unit {
            using nano = std::nano;
            using micro = std::micro;
            using milli = std::milli;
            using seconds = std::ratio<1>;
            using minutes = std::ratio<60>;
            using hours = std::ratio<3600>;
        };

        struct Point {
            static Point Now() {
                return {std::chrono::high_resolution_clock::now()};
            }

            template<typename Duration = Unit::milli>
            double duration(const Point &other) const {
                return std::chrono::duration<double, Duration>(value - other.value).count();
            }

            template<typename Duration = Unit::milli>
            std::string unit_string() const {
                return DurationTypeToString<Duration>::value();
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> value;
        };

        Point engine_run_start;
        Point engine_run_end;
        Point engine_constructor_start;
        Point engine_constructor_end;

        struct Mainloop {
            Point current;
            Point last;
            double avg_duration = 0;
            double duration = 0;
            size_t iter_counter = 0;
            int fps = 0;
            int avg_fps = 0;
        } mainloop;
    };

    template<>
    struct DurationTypeToString<Time::Unit::nano> {
        static std::string value() { return " ns"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::micro> {
        static std::string value() { return " µs"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::milli> {
        static std::string value() { return " ms"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::seconds> {
        static std::string value() { return "  s"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::minutes> {
        static std::string value() { return "min"; }
    };

    template<>
    struct DurationTypeToString<Time::Unit::hours> {
        static std::string value() { return "  h"; }
    };

    struct WorkerPool {
        std::vector<std::thread> workers;
        std::queue<std::shared_ptr<Command>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop = true;
    };

    class Entity {
    public:
        Entity(entt::registry *registry, entt::entity id) : registry(registry), id(id) {}

        virtual ~Entity() = default;

        entt::registry *registry;
        entt::entity id;
    };

    template<typename Key, typename Value>
    struct Cache : public std::unordered_map<Key, Value> {
        using std::unordered_map<Key, Value>::unordered_map;
    };
}

#endif //ENGINE23_COMPONENTS_H
