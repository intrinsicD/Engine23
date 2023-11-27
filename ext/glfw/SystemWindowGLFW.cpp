//
// Created by alex on 27.11.23.
//

#include "SystemWindow.h"
#include "Engine.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "SystemGui.h"

namespace Bcg::System::Window {
    void add_system() {
        auto *engine = Engine::Instance();

        if (!glfwInit()) {
            Log::Error("SystemWindow: Failed to initialize GLFW");
            return;
        } else {
            Log::Info("SystemWindow: GLFW initialized");
            glfwSetErrorCallback([](int error, const char *description) {
                Log::Error("GLFW: Error(" + std::to_string(error) + "): " + std::string(description)).execute();
            });
        }

        engine->dispatcher.sink<Events::Startup<Engine>>().connect<System::Window::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().connect<System::Window::on_shutdown_engine>();

        Log::Info("SystemWindow: Added");
    }

    void remove_system() {
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Startup<Engine>>().disconnect<System::Window::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<System::Window::on_shutdown_engine>();
        Log::Info("SystemWindow: Removed");

        glfwTerminate();
    }

    void on_startup_engine(const Events::Startup<Engine> &event) {
        if (Engine::Instance()->state.ctx().find<WindowConfig>()) {
            Log::Warn("SystemWindow: Already started");
            return;
        } else {
            Log::Info("SystemWindow: Started");
            Engine::Instance()->state.ctx().emplace<WindowConfig>();
        }
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Begin<Frame>>().connect<System::Window::on_begin_frame>();
        engine->dispatcher.sink<Events::End<Frame>>().connect<System::Window::on_end_frame>();

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

        auto *window = glfwCreateWindow(window_config.width, window_config.height, window_config.title.c_str(),
                                        nullptr, nullptr);
        glfwSetWindowUserPointer(window, engine);
        glfwMakeContextCurrent(window);

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
            if (action == GLFW_PRESS) {
                input.keyboard.keys[key] = true;
            } else if (action == GLFW_RELEASE) {
                input.keyboard.keys[key] = false;
            }
        });

        Gui::add_to_window(window);
    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
        if (!Engine::Instance()->state.ctx().find<WindowConfig>()) {
            Log::Error("SystemWindow: Not started");
            return;
        } else {
            Log::Info("SystemWindow: Stopped");
            Engine::Instance()->state.ctx().erase<WindowConfig>();
        }
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Begin<Frame>>().disconnect<System::Window::on_begin_frame>();
        engine->dispatcher.sink<Events::End<Frame>>().disconnect<System::Window::on_end_frame>();

        glfwDestroyWindow(glfwGetCurrentContext());
    }

    void on_begin_frame(const Events::Begin<Frame> &event) {

    }

    void on_end_frame(const Events::End<Frame> &event) {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }
}