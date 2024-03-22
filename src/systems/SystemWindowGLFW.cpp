//
// Created by alex on 27.11.23.
//

#include "systems/SystemWindowGLFW.h"
#include "Engine.h"
#include "Events.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "systems/SystemDearImGui.h"
#include "Window.h"
#include "Viewport.h"
#include "Input.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemWindowGLFWInternal {
        void on_end_main_loop(const Events::End<MainLoop> &event);

        double GetDpiScale();

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
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
            if (!Engine::Instance()->state.ctx().find<Window>()) {
                Log::Info(SystemWindowGLFW::name(), "Startup").enqueue();
                Engine::Instance()->state.ctx().emplace<Window>();
            }
            auto *engine = Engine::Instance();

            auto &window = engine->state.ctx().get<Window>();

            window.dpi = GetDpiScale();
            Log::Info(SystemWindowGLFW::name() + ": Detected DPI: " + std::to_string(window.dpi)).enqueue();


            auto *h_window = glfwCreateWindow(window.width, window.height, window.title.c_str(),
                                            nullptr, nullptr);
            glfwSetWindowUserPointer(h_window, engine);
            glfwMakeContextCurrent(h_window);
            glfwSwapInterval(1);

            glfwSetWindowCloseCallback(h_window, [](GLFWwindow *h_window) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                engine->is_running = false;
            });
            glfwSetWindowSizeCallback(h_window, [](GLFWwindow *h_window, int width, int height) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &window = engine->state.ctx().get<Window>();
                window.width = width;
                window.height = height;
                engine->dispatcher.trigger(Events::Update<Viewport>{});
            });
            glfwSetMouseButtonCallback(h_window, [](GLFWwindow *h_window, int button, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    input.mouse.button.left = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                    input.mouse.button.right = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                    input.mouse.button.middle = action == GLFW_PRESS;
                }
                engine->dispatcher.trigger(Events::Update<Input::Mouse::Button>{});
            });
            glfwSetCursorPosCallback(h_window, [](GLFWwindow *h_window, double xpos, double ypos) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.position_delta[0] = xpos - input.mouse.position[0];
                input.mouse.position_delta[1] = ypos - input.mouse.position[1];
                input.mouse.position[0] = xpos;
                input.mouse.position[1] = ypos;
                engine->dispatcher.trigger(Events::Update<Input::Mouse::Position>{});
            });
            glfwSetScrollCallback(h_window, [](GLFWwindow *h_window, double xoffset, double yoffset) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.scroll[0] = xoffset;
                input.mouse.scroll[1] = yoffset;
                engine->dispatcher.trigger(Events::Update<Input::Mouse::Scroll>{});
            });
            glfwSetKeyCallback(h_window, [](GLFWwindow *h_window, int key, int scancode, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                if (key >= input.keyboard.keys.size()) input.keyboard.keys.resize(key + 1);
                if(GLFW_REPEAT == action) return;
                input.keyboard.keys[key] = action == GLFW_PRESS;
                engine->dispatcher.trigger(Events::Update<Input::Keyboard>{});
                if(action == GLFW_PRESS){
                    Log::Info("Key: " + std::to_string(key) + " Press").enqueue();
                }else{
                    Log::Info("Key: " + std::to_string(key) + " Release").enqueue();
                };
            });
            glfwSetDropCallback(h_window, [](GLFWwindow *h_window, int count, const char **paths) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.drop.paths.clear();
                for (int i = 0; i < count; ++i) {
                    input.drop.paths.emplace_back(paths[i]);
                }
                engine->dispatcher.trigger(Events::Update<Input::Drop>{});
                input.drop.paths.clear();
            });

            engine->dispatcher.trigger(Events::Startup<Renderer>{});

            SystemGui().add_to_window(h_window);
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.trigger(Events::Shutdown<Renderer>{});
            if (!Engine::Instance()->state.ctx().find<Window>()) {
                Log::Error(SystemWindowGLFW::name() + ": Not started").enqueue();
                return;
            } else {
                Log::Info(SystemWindowGLFW::name() + ": Stopped").enqueue();
                Engine::Instance()->state.ctx().erase<Window>();
            }
            glfwDestroyWindow(glfwGetCurrentContext());
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemWindowGLFW::name(){
        return "SystemWindowGLFW";
    }

    void SystemWindowGLFW::swap_and_poll_events() {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }

    void SystemWindowGLFW::pre_init() {
        Engine::Context().emplace<Window>();
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

        Log::Info("Initialized", name()).enqueue();
    }

    void SystemWindowGLFW::remove() {
        auto *engine = Engine::Instance();
        engine->dispatcher.sink<Events::Startup<Engine>>().disconnect<SystemWindowGLFWInternal::on_startup_engine>();
        engine->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<SystemWindowGLFWInternal::on_shutdown_engine>();
        engine->dispatcher.sink<Events::End<MainLoop>>().disconnect<SystemWindowGLFWInternal::on_end_main_loop>();
        Log::Info("Removed", name()).enqueue();

        glfwTerminate();
    }
}