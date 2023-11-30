//
// Created by alex on 27.11.23.
//

#include "SystemWindowGLFW.h"
#include "Engine.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "SystemDearImGui.h"

namespace Bcg::System::Window::Glfw {

    void on_end_main_loop(const Events::End<MainLoop> &event) {
        swap_and_poll_events();
    }

    void add_system() {
        auto *engine = Engine::Instance();

        glfwSetErrorCallback([](int error, const char *description) {
            Log::Error("GLFW: Error(" + std::to_string(error) + "): " + std::string(description)).enqueue();
        });

        if (!glfwInit()) {
            Log::Error("SystemWindow: Failed to initialize GLFW").enqueue();
            return;
        }

        engine->dispatcher.sink<Events::Startup<Engine>>().connect<System::Window::Glfw::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().connect<System::Window::Glfw::on_shutdown_engine>();
        engine->dispatcher.sink<Events::End<MainLoop>>().connect<System::Window::Glfw::on_end_main_loop>();

        Log::Info("SystemWindow: Added").enqueue();
    }

    void remove_system() {
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Startup<Engine>>().disconnect<System::Window::Glfw::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<System::Window::Glfw::on_shutdown_engine>();
        Log::Info("SystemWindow: Removed").enqueue();

        glfwTerminate();
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
            Log::Warn("SystemWindow: Already started").enqueue();
            return;
        } else {
            Log::Info("SystemWindow: Started").enqueue();
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
        Log::Info("SystemWindow: Detected DPI: " + std::to_string(window_config.dpi)).enqueue();


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
        });

        Gui::add_to_window(window);
    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
        if (!Engine::Instance()->state.ctx().find<WindowConfig>()) {
            Log::Error("SystemWindow: Not started").enqueue();
            return;
        } else {
            Log::Info("SystemWindow: Stopped").enqueue();
            Engine::Instance()->state.ctx().erase<WindowConfig>();
        }
        glfwDestroyWindow(glfwGetCurrentContext());
    }

    void swap_and_poll_events() {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }
}