//
// Created by alex on 27.11.23.
//

#include "Window/SystemWindowGLFW.h"
#include "Engine.h"
#include "Events/Events.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "systems/SystemDearImGui.h"
#include "Window.h"
#include "Viewport.h"
#include "Input.h"
#include "imgui.h"
#include "Components.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemWindowGLFWInternal {
        static bool show_gui = false;

        void on_end_main_loop(const Events::End<MainLoop> &event);

        double GetDpiScale();

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemWindowGLFWInternal {

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Window", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Window", &show_gui)) {
                Components<Window> windows(SystemWindowGLFW::component_name());
                auto &component_window = Engine::Context().get<Component<Window>>();
                auto &window = windows.get_instance(component_window);
                auto size = window.get_size();
                ImGui::LabelText("Title", "%s", window.title.c_str());
                ImGui::LabelText("Size", "%d x %d", size[0], size[1]);
                ImGui::LabelText("Dpi", "%lf", window.dpi);
                ImGui::LabelText("WindowHandle", "%d", window.window_handle);
                ImGui::LabelText("Aspect", "%lf", size[0] / double(size[1]));
                ImGui::ColorEdit4("Background", window.background_color.data());
            }
            ImGui::End();
        }

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
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);
            Log::Info(SystemWindowGLFW::name(), "Startup").enqueue();

            window.dpi = GetDpiScale();
            Log::Info(SystemWindowGLFW::name() + ": Detected DPI: " + std::to_string(window.dpi)).enqueue();

            window.window_handle = glfwCreateWindow(window.create_width, window.create_height, window.title.c_str(),
                                                    nullptr, nullptr);

            auto *glfw_handle = static_cast<GLFWwindow *>(window.window_handle);
            glfwSetWindowUserPointer(glfw_handle, Engine::Instance());
            glfwMakeContextCurrent(glfw_handle);
            glfwSwapInterval(1);

            glfwSetWindowCloseCallback(glfw_handle, [](GLFWwindow *h_window) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                engine->is_running = false;
            });
            glfwSetWindowSizeCallback(glfw_handle, [](GLFWwindow *h_window, int width, int height) {
                Engine::Dispatcher().trigger(Events::Update<Viewport>{});
            });
            glfwSetMouseButtonCallback(glfw_handle, [](GLFWwindow *h_window, int button, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    input.mouse.button.left = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                    input.mouse.button.right = action == GLFW_PRESS;
                } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                    input.mouse.button.middle = action == GLFW_PRESS;
                }
                Engine::Dispatcher().trigger(Events::Update<Input::Mouse::Button>{});
            });
            glfwSetCursorPosCallback(glfw_handle, [](GLFWwindow *h_window, double xpos, double ypos) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.position_delta[0] = xpos - input.mouse.position[0];
                input.mouse.position_delta[1] = ypos - input.mouse.position[1];
                input.mouse.position[0] = xpos;
                input.mouse.position[1] = ypos;
                Engine::Dispatcher().trigger(Events::Update<Input::Mouse::Position>{});
            });
            glfwSetScrollCallback(glfw_handle, [](GLFWwindow *h_window, double xoffset, double yoffset) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.mouse.scroll[0] = xoffset;
                input.mouse.scroll[1] = yoffset;
                Engine::Dispatcher().trigger(Events::Update<Input::Mouse::Scroll>{});
            });
            glfwSetKeyCallback(glfw_handle, [](GLFWwindow *h_window, int key, int scancode, int action, int mods) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                if (key >= input.keyboard.keys.size()) input.keyboard.keys.resize(key + 1);
                if (GLFW_REPEAT == action) return;
                input.keyboard.keys[key] = action == GLFW_PRESS;
                Engine::Dispatcher().trigger(Events::Update<Input::Keyboard>{});
                if (action == GLFW_PRESS) {
                    Log::Info("Key: " + std::to_string(key) + " Press").enqueue();
                } else {
                    Log::Info("Key: " + std::to_string(key) + " Release").enqueue();
                };
            });
            glfwSetDropCallback(glfw_handle, [](GLFWwindow *h_window, int count, const char **paths) {
                auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
                auto &input = engine->state.ctx().get<Input>();
                input.drop.paths.clear();
                for (int i = 0; i < count; ++i) {
                    input.drop.paths.emplace_back(paths[i]);
                }
                Engine::Dispatcher().trigger(Events::Update<Input::Drop>{});
                input.drop.paths.clear();
            });

            Engine::Dispatcher().trigger(Events::Startup<Renderer>{});

            SystemGui().add_to_window(glfw_handle);
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().trigger(Events::Shutdown<Renderer>{});
            if (!Engine::Context().find<Window>()) {
                Log::Error(SystemWindowGLFW::name() + ": Not started").enqueue();
                return;
            } else {
                Log::Info(SystemWindowGLFW::name() + ": Stopped").enqueue();
                Engine::Context().erase<Window>();
            }
            glfwDestroyWindow(glfwGetCurrentContext());
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemWindowGLFW::name() {
        return "System" + component_name();
    }


    std::string SystemWindowGLFW::component_name() {
        return "WindowGLFW";
    }

    void SystemWindowGLFW::swap_and_poll_events() {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }

    void SystemWindowGLFW::pre_init() {
        Engine::Context().emplace<Window>();
        Components<Window> windows(component_name());
        auto window_id = windows.create_instance();
        Engine::Context().emplace<Component<Window>>(window_id);
    }

    void SystemWindowGLFW::init() {
        glfwSetErrorCallback([](int error, const char *description) {
            Log::Error("GLFW: Error(" + std::to_string(error) + "): " + std::string(description)).enqueue();
        });

        if (!glfwInit()) {
            Log::Error(name() + ": Failed to initialize GLFW").enqueue();
            return;
        }

        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<SystemWindowGLFWInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<SystemWindowGLFWInternal::on_shutdown_engine>();
        Engine::Dispatcher().sink<Events::End<MainLoop>>().connect<SystemWindowGLFWInternal::on_end_main_loop>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<SystemWindowGLFWInternal::on_update_gui_menu>();

        Log::Info("Initialized", name()).enqueue();
    }

    void SystemWindowGLFW::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<SystemWindowGLFWInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<SystemWindowGLFWInternal::on_shutdown_engine>();
        Engine::Dispatcher().sink<Events::End<MainLoop>>().disconnect<SystemWindowGLFWInternal::on_end_main_loop>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<SystemWindowGLFWInternal::on_update_gui_menu>();

        Log::Info("Removed", name()).enqueue();

        glfwTerminate();
    }
}