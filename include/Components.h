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
#include <cmath>
#include "glm/glm.hpp"
#include "entt/fwd.hpp"

namespace Bcg {
    struct StartupWindowConfig {
        std::string title = "Viewer";
        int width = 800;
        int height = 600;

        template<typename T>
        T get_aspect() const {
            return static_cast<T>(width) / static_cast<T>(height);
        }
    };

    struct Viewport {
        int x = 0;
        int y = 0;
        int width = 800;
        int height = 600;
    };

    struct WindowConfig {
        std::string title = "Viewer";
        int width = 800;
        int height = 600;
        double dpi = 1.0;
        float background_color[4] = {0.1f, 0.3f, 0.6f, 1.0f};

        template<typename T>
        T get_aspect() const {
            return static_cast<T>(width) / static_cast<T>(height);
        }
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
            struct Position : public glm::vec2 {
            };
            struct Button {
                bool left = false;
                bool right = false;
                bool middle = false;
            };
            struct Scroll : public glm::vec2 {
            };
            Position position;
            Position position_delta;
            Position last_left_click;
            Position last_middle_click;
            Position last_right_click;
            Position last_drag_pos;
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

    struct Renderable {
        virtual void update_local_uniforms(unsigned int shader_id) const = 0;

        virtual void draw() const = 0;
    };

    struct RenderBatch {
        std::string name;

        virtual void update_global_uniforms(unsigned int shader_id) const = 0;

        std::vector<std::shared_ptr<Renderable>> renderables;
    };

    struct RenderBatches {
        std::unordered_map<unsigned int, std::shared_ptr<RenderBatch>> batches;
    };

    struct Faces {
        std::vector<glm::uvec3> vertices;
        std::vector<glm::uvec3> normals;
        std::vector<glm::uvec3> texcoords;
    };

    struct Vertices {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
    };

    struct Mesh {
        Vertices vertices;
        Faces faces;
    };

    inline auto dims(const glm::vec2 &v) -> size_t {
        return 2;
    }

    inline auto dims(const glm::uvec2 &v) -> size_t {
        return 2;
    }

    inline auto dims(const glm::vec3 &v) -> size_t {
        return 3;
    }

    inline auto dims(const glm::uvec3 &v) -> size_t {
        return 3;
    }

    inline auto dims(const glm::vec4 &v) -> size_t {
        return 4;
    }

    template<typename T>
    inline auto dims(const std::vector<T> &v) -> size_t {
        return dims(v[0]);
    }

    struct Transform {
        glm::mat4 model;

        glm::vec3 get_position() const;

        glm::vec3 get_scale() const;

        glm::quat get_rotation() const;

        glm::vec3 get_euler_angles() const;

        glm::vec3 get_angles_axis() const;

        glm::vec3 get_x_axis() const;

        glm::vec3 get_y_axis() const;

        glm::vec3 get_z_axis() const;

        void set_position(const glm::vec3 &position);

        void set_scale(const glm::vec3 &scale);

        void set_rotation(const glm::quat &rotation);

        void set_rotation(const glm::vec3 &axis, float angle);

        void set_rotation(const glm::vec3 &euler_angles);

        void set_rotation(float pitch, float yaw, float roll);
    };

    struct Camera {
        struct ProjectionParameters {
            struct Perspective {
                float fovy = 45.0f;
                float aspect = 4.0f / 3.0f;
                float near;
                float far;
                bool dirty;
            } perspective_parameters;
            struct Orthographic {
                float left;
                float right;
                float bottom;
                float top;
                float near;
                float far;
                bool dirty;
            } orthographic_parameters;
        } projection_parameters;
        struct ViewParameters{
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
            glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
            bool dirty;
        }view_parameters;

        struct Sensitivity{
            float zoom = 5.0f;
            float move = 5.0f;
            float rotate = 5.0f;
            float drag = 0.01f;
        }sensitivity;

        bool is_orthographic = true;

        Camera();

        glm::mat4 get_model() const;

        glm::mat4 get_view() const;

        glm::mat4 get_projection() const;

        void set_front(const glm::vec3 &front);

        void set_target(const glm::vec3 &target);

        void set_position(const glm::vec3 &position);

        void set_worldup(const glm::vec3 &world_up);

        void set_view_parameters(const ViewParameters &parameters);

        void set_perspective_parameters(const ProjectionParameters::Perspective &parameters);

        void set_orthographic_parameters(const ProjectionParameters::Orthographic &parameters);
    };

    struct ArcBallCameraController {
        Camera &camera;
        bool last_point_ok = false;
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec2 last_point_2d = glm::vec2(0.0f);
        glm::vec3 last_point_3d = glm::vec3(0.0f);
    };

    struct AABB {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

        void grow(const glm::vec3 &point);

        void merge(const AABB &aabb);

        glm::vec3 get_center() const;

        glm::vec3 get_extent() const;

        void set(const glm::vec3 &center, const glm::vec3 &extent);
    };
}

#endif //ENGINE23_COMPONENTS_H
