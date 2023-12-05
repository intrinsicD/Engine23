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
        double dpi = 1.0;
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
        struct Drop {
            std::vector<std::string> paths;
        } drop;
    };

    struct Frame {
    };
    struct Command;

    struct CommandBuffer : public std::vector<std::shared_ptr<Command>> {
        using std::vector<std::shared_ptr<Command>>::vector;

        void clear_and_swap() {
            std::vector<std::shared_ptr<Command>>().swap(*this);
        }
    };

    struct CommandBufferCurrent : public CommandBuffer {
        using CommandBuffer::CommandBuffer;
    };

    struct CommandBufferNext : public CommandBuffer {
        using CommandBuffer::CommandBuffer;
    };

    struct CommandDoubleBuffer {
        CommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : p_current(current), p_next(next) {}

        CommandBuffer *p_current;
        CommandBuffer *p_next;
        std::mutex currentMutex;
        std::mutex nextMutex;

        void enqueue_current(std::shared_ptr<Command> command) {
            std::unique_lock<std::mutex> lock(currentMutex);
            p_current->emplace_back(std::move(command));
        }

        void enqueue_next(std::shared_ptr<Command> command) {
            std::unique_lock<std::mutex> lock(nextMutex);
            p_next->emplace_back(std::move(command));
        }

        void clear_and_swap() {
            std::unique_lock<std::mutex> lock_curr(currentMutex);
            std::unique_lock<std::mutex> lock_next(nextMutex);
            p_current->clear_and_swap();
            std::swap(p_current, p_next);
        }
    };

    struct RenderCommandDoubleBuffer : public CommandDoubleBuffer {
        RenderCommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : CommandDoubleBuffer(current, next) {}
    };

    struct SimulationCommandDoubleBuffer : public CommandDoubleBuffer {
        SimulationCommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : CommandDoubleBuffer(current,
                                                                                                         next) {}
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

        struct SimulationLoop {
            Point start;
            double avg_duration = 0;
            double duration = 0;
            size_t iter_counter = 0;
            double min_step_size = 1.0 / 30000;
            double accumulator = 0;
        } simulationloop;
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
        bool stop = false;
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

    struct FileWatcher {
        Cache<std::string, std::vector<std::function<void()>>> items;
        Cache<std::string, std::chrono::file_clock::time_point> last_write_times;

        void add(std::string filepath, std::function<void()> callback);

        void remove(std::string filepath);

        void clear();

        void check();
    };
}

#endif //ENGINE23_COMPONENTS_H
