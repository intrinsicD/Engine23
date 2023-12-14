//
// Created by alex on 27.11.23.
//

#include "SystemWindowGLFW.h"
#include "Engine.h"
#include "Events.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "SystemDearImGui.h"

namespace Bcg {
    namespace SystemWindowGLFWInternal {
        void on_end_main_loop(const Events::End<MainLoop> &event) {
            SystemWindowGLFW::swap_and_poll_events();
        }

        double GetDpiScale() {
            // Get the primary monitor
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            if (!monitor) {
                return 1.0; // Default scale factor
            }

            // Get monitor physical size
            int widthMM, heightMM;
            glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
            if (widthMM == 0 || heightMM == 0) {
                return 1.0; // Default scale factor
            }

            // Get monitor resolution
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (!mode) {
                return 1.0; // Default scale factor
            }
            int widthPx = mode->width;
            int heightPx = mode->height;

            // Calculate DPI
            double dpiX = static_cast<float>(widthPx) / (static_cast<float>(widthMM) / 25.4);
            double dpiY = static_cast<float>(heightPx) / (static_cast<float>(heightMM) / 25.4);

            // Average and normalize DPI to get scale factor
            // Here, 96 is the standard DPI value.
            double dpiScale = (dpiX + dpiY) / 2.0 / 96.0;

            return dpiScale;
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            if (Engine::Instance()->state.ctx().find<WindowConfig>()) {
                Log::Warn(SystemWindowGLFW::name() + ": Already started").enqueue();
                return;
            } else {
                Log::Info(SystemWindowGLFW::name() + ": Started").enqueue();
                Engine::Instance()->state.ctx().emplace<WindowConfig>();
            }
            auto *engine = Engine::Instance();

            auto *startup_window_config = engine->state.ctx().find<StartupWindowConfig>();
            auto &window_config = engine->state.ctx().get<WindowConfig>();
            if (startup_window_config) {
                window_config.title = startup_window_config->title;
                window_config.width = startup_window_config->width;
                window_config.height = startup_window_config->height;
            } else {
                window_config.title = "Viewer";
                window_config.width = 800;
                window_config.height = 600;
            }

            window_config.dpi = GetDpiScale();
            Log::Info(SystemWindowGLFW::name() + ": Detected DPI: " + std::to_string(window_config.dpi)).enqueue();


            auto *window = glfwCreateWindow(window_config.width, window_config.height, window_config.title.c_str(),
                                            nullptr, nullptr);
            glfwSetWindowUserPointer(window, engine);
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);

            glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                engine->is_running = false;
            });
            glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &window_config = engine->state.ctx().get<WindowConfig>();
                window_config.width = width;
                window_config.height = height;
                window_config.aspect_ratio = static_cast<double>(width) / static_cast<double>(height);
            });
            glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &input = engine->state.ctx().get<Input>();
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    input.mouse.button.left = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                    input.mouse.button.right = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                    input.mouse.button.middle = action == GLFW_PRESS;
                }
            });
            glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.position.x = xpos;
                input.mouse.position.y = ypos;
            });
            glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.scroll.x = xoffset;
                input.mouse.scroll.y = yoffset;
            });
            glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &input = engine->state.ctx().get<Input>();
                if (key >= input.keyboard.keys.size()) input.keyboard.keys.resize(key + 1);
                input.keyboard.keys[key] = action == GLFW_PRESS;
            });
            glfwSetDropCallback(window, [](GLFWwindow *window, int count, const char **paths) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                auto &input = engine->state.ctx().get<Input>();
                input.drop.paths.clear();
                for (int i = 0; i < count; ++i) {
                    input.drop.paths.emplace_back(paths[i]);
                }
                engine->dispatcher.trigger(Events::Update<Input::Drop>{});
            });

            engine->dispatcher.trigger(Events::Startup<Renderer>{});

            SystemGui().add_to_window(window);
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.trigger(Events::Shutdown<Renderer>{});
            if (!Engine::Instance()->state.ctx().find<WindowConfig>()) {
                Log::Error(SystemWindowGLFW::name() + ": Not started").enqueue();
                return;
            } else {
                Log::Info(SystemWindowGLFW::name() + ": Stopped").enqueue();
                Engine::Instance()->state.ctx().erase<WindowConfig>();
            }
            glfwDestroyWindow(glfwGetCurrentContext());
        }
    }

    std::string SystemWindowGLFW::name(){
        return "SystemWindowGLFW";
    }

    void SystemWindowGLFW::pre_init() {
        Engine::Context().emplace<StartupWindowConfig>();
    }

    void SystemWindowGLFW::init() {
        auto *engine = Engine::Instance();

        glfwSetErrorCallback([](int error, const char *description) {
            Log::Error("GLFW: Error(" + std::to_string(error) + "): " + std::string(description)).enqueue();
        });

        if (!glfwInit()) {
            Log::Error(name() + ": Failed to initialize GLFW").enqueue();
            return;
        }

        engine->dispatcher.sink<Events::Startup<Engine>>().connect<SystemWindowGLFWInternal::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().connect<SystemWindowGLFWInternal::on_shutdown_engine>();
        engine->dispatcher.sink<Events::End<MainLoop>>().connect<SystemWindowGLFWInternal::on_end_main_loop>();

        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemWindowGLFW::remove() {
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Startup<Engine>>().disconnect<SystemWindowGLFWInternal::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<SystemWindowGLFWInternal::on_shutdown_engine>();
        engine->dispatcher.sink<Events::End<MainLoop>>().disconnect<SystemWindowGLFWInternal::on_end_main_loop>();
        Log::Info(name() + ": Removed").enqueue();

        glfwTerminate();
    }

    void SystemWindowGLFW::swap_and_poll_events() {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }
}