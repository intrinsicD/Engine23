//
// Created by alex on 23.11.23.
//

#include "WindowManagerGLFW.h"
#include "Factories.h"
#include "GLFW/glfw3.h"
#include "Engine.h"
#include <memory>

namespace Bcg {
    struct GLFWInit {
        int init;
    };

    GLFWwindow *glfw(void *handle) {
        return static_cast<GLFWwindow *>(handle);
    }

    WindowManagerGLFW::WindowManagerGLFW() : WindowManager() {
        if (!Engine::State().all_of<GLFWInit>(id)) {
            if (!glfwInit()) {
                throw std::runtime_error("Failed to initialize GLFW");
            } else {
                Engine::State().emplace<GLFWInit>(id);
            }
            auto *command_buffer_manager = ManagerFactory::create_or_get_command_buffer_manager();
            command_buffer_manager->push_command_to_shutdown(std::make_shared<TaskCommand>("Shutdown GLFW", []() {
                auto *window_manager = Engine::Context().find<WindowManagerGLFW>();
                glfwDestroyWindow(glfw(window_manager->window->handle));
                glfwTerminate();
                return 1;
            }));
        }
    }

    void WindowManagerGLFW::create_window(int width, int height, std::string title) {
        if (!Engine::State().all_of<Window>(id)) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            WindowManager::create_window(width, height, std::move(title));
            window->handle = glfwCreateWindow(width, height, window->title.c_str(), nullptr, nullptr);
            glfwSetWindowUserPointer(glfw(window->handle), Engine::Instance());
            glfwSetWindowCloseCallback(glfw(window->handle), [](GLFWwindow *window) {
                glfwSetWindowShouldClose(window, true);
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
                engine->is_running = false;
            });
        }
    }

    void WindowManagerGLFW::destroy_window() {
        glfwDestroyWindow(glfw(window->handle));
        glfwTerminate();
    }

    void WindowManagerGLFW::resize_window(int width, int height) const {
        glfwSetWindowSize(glfw(window->handle), width, height);
    }

    void WindowManagerGLFW::set_window_title(std::string title) const {
        glfwSetWindowTitle(glfw(window->handle), title.c_str());
    }

    double WindowManagerGLFW::get_aspect_ratio() const {
        int width, height;
        glfwGetWindowSize(glfw(window->handle), &width, &height);
        return static_cast<double>(width) / static_cast<double>(height);
    }

    void WindowManagerGLFW::begin_frame() const {
        glfwPollEvents();
    }

    void WindowManagerGLFW::end_frame() const {
        glfwSwapBuffers(glfw(window->handle));
    }
}