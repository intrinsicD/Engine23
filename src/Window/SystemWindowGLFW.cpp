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
#include "Mouse.h"
#include "Keyboard.h"
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

            window.window_handle = glfwCreateWindow(window.width, window.height, window.title.c_str(),nullptr, nullptr);

            auto *glfw_handle = static_cast<GLFWwindow *>(window.window_handle);
            auto &window_register = Engine::Context().get<WindowRegister>();
            window_register[glfw_handle] = component_window.index;

            glfwSetWindowUserPointer(glfw_handle, Engine::Instance());
            glfwMakeContextCurrent(glfw_handle);
            glfwSwapInterval(1);

            glfwSetWindowCloseCallback(glfw_handle, [](GLFWwindow *h_window) {
                SystemWindowGLFW::set_window_close(h_window);
            });
            glfwSetWindowSizeCallback(glfw_handle, [](GLFWwindow *h_window, int width, int height) {
                SystemWindowGLFW::set_window_resize(h_window, width, height);
            });
            glfwSetMouseButtonCallback(glfw_handle, [](GLFWwindow *h_window, int button, int action, int mods) {
                SystemWindowGLFW::set_mouse_button(button, action, mods);
            });
            glfwSetCursorPosCallback(glfw_handle, [](GLFWwindow *h_window, double xpos, double ypos) {
                SystemWindowGLFW::set_mouse_cursor_pos(h_window, xpos, ypos);
            });
            glfwSetScrollCallback(glfw_handle, [](GLFWwindow *h_window, double xoffset, double yoffset) {
                SystemWindowGLFW::set_mouse_scroll(h_window, xoffset, yoffset);
            });
            glfwSetKeyCallback(glfw_handle, [](GLFWwindow *h_window, int key, int scancode, int action, int mods) {
                if (GLFW_REPEAT == action) return;
                SystemWindowGLFW::set_keyboard(h_window, key, scancode, action, mods);
            });
            glfwSetDropCallback(glfw_handle, [](GLFWwindow *h_window, int count, const char **paths) {
                SystemWindowGLFW::set_drop(h_window, count, paths);
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

    void SystemWindowGLFW::set_window_close(void *window_handle) {
        Engine::Instance()->is_running = false;

        if (Engine::Instance()->window_close_callback) {
            Engine::Instance()->window_close_callback();
        }
        Engine::Dispatcher().trigger<Events::Internal::Callback::WindowClose>();
    }

    void SystemWindowGLFW::set_window_resize(void *window_handle, int width, int height) {
        auto &window_register = Engine::Context().get<WindowRegister>();
        auto &windwo_id = window_register[window_handle];
        Components<Window> windows(SystemWindowGLFW::component_name());
        auto &window = windows.get_instance(windwo_id);
        window.width = width;
        window.height = height;

        if (Engine::Instance()->window_size_callback) {
            Engine::Instance()->window_size_callback();
        }
        Engine::Dispatcher().trigger<Events::Internal::Callback::WindowResize>();
        //TODO cleanup events... pick which one is triggered
        Engine::Dispatcher().trigger(Events::Update<Viewport>{});
    }

    void SystemWindowGLFW::set_mouse_button(int button, int action, int mods) {
        auto &mouse = Engine::Context().get<Mouse<float>>();
        mouse.button.button = button;
        mouse.button.action = action;
        mouse.button.mods = mods;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            mouse.button.left = action == GLFW_PRESS;
            if (mouse.button.left) {
                mouse.position.last_left_click = mouse.position.current;
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            mouse.button.right = action == GLFW_PRESS;
            if (mouse.button.right) {
                mouse.position.last_right_click = mouse.position.current;
            }
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            mouse.button.middle = action == GLFW_PRESS;
            if (mouse.button.middle) {
                mouse.position.last_middle_click = mouse.position.current;
            }
        }

        if (!mouse.button.any()) {
            mouse.state = Mouse<float>::State::IDLE;
        }

        if (Engine::Instance()->mouse_button_callback) {
            Engine::Instance()->mouse_button_callback();
        }

        Engine::Dispatcher().trigger(Events::Update<Mouse<float>::Button>{});

        if (action == GLFW_PRESS) {
            Engine::Dispatcher().trigger(Events::Update<Mouse<float>::Button::Press>{});
        } else if (action == GLFW_RELEASE) {
            Engine::Dispatcher().trigger(Events::Update<Mouse<float>::Button::Release>{});
        }
    }

    void SystemWindowGLFW::set_mouse_cursor_pos(void *window_handle, double xpos, double ypos) {
        auto &mouse = Engine::Context().get<Mouse<float>>();

        if (mouse.button.any()) {
            mouse.position.delta[0] = xpos - mouse.position.current[0];
            mouse.position.delta[1] = ypos - mouse.position.current[1];
            mouse.state = Mouse<float>::State::DRAG;
        } else {
            mouse.state = Mouse<float>::State::MOVE;
        }

        mouse.position.current[0] = xpos;
        mouse.position.current[1] = ypos;

        if (Engine::Instance()->cursor_pos_callback) {
            Engine::Instance()->cursor_pos_callback();
        }

        Engine::Dispatcher().trigger(Events::Update<Mouse<float>::Position>{});
    }

    void SystemWindowGLFW::set_mouse_scroll(void *window_handle, double xoffset, double yoffset) {
        auto &mouse = Engine::Context().get<Mouse<float>>();

        mouse.scroll[0] = xoffset;
        mouse.scroll[1] = yoffset;

        if (Engine::Instance()->scroll_callback) {
            Engine::Instance()->scroll_callback();
        }

        Engine::Dispatcher().trigger(Events::Update<Mouse<float>::Scroll>{});
    }

    void SystemWindowGLFW::set_keyboard(void *window_handle, int key, int scancode, int action, int mods) {
        auto &keyboard = Engine::Context().get<Keyboard>();
        keyboard.key = key;
        keyboard.scancode = scancode;
        keyboard.action = action;
        keyboard.mods = mods;

        bool is_pressed = action == GLFW_PRESS;
        bool is_released = action == GLFW_RELEASE;

        if (is_pressed) {
            keyboard.keys[key] = true;
            ++keyboard.count_pressed;
        } else if (is_released) {
            keyboard.keys[key] = false;
            keyboard.count_pressed = std::max(0, keyboard.count_pressed - 1);
        }

        if (action == GLFW_PRESS) {
            Log::Info("Key: " + std::to_string(key) + " Press").enqueue();
        } else {
            Log::Info("Key: " + std::to_string(key) + " Release").enqueue();
        };

        switch (key) {
            case GLFW_KEY_A: {
                keyboard.set_key(Keyboard::KEY::_A, is_pressed);
                break;
            }
            case GLFW_KEY_B: {
                keyboard.set_key(Keyboard::KEY::_B, is_pressed);
                break;
            }
            case GLFW_KEY_C: {
                keyboard.set_key(Keyboard::KEY::_C, is_pressed);
                break;
            }
            case GLFW_KEY_D: {
                keyboard.set_key(Keyboard::KEY::_D, is_pressed);
                break;
            }
            case GLFW_KEY_E: {
                keyboard.set_key(Keyboard::KEY::_E, is_pressed);
                break;
            }
            case GLFW_KEY_F: {
                keyboard.set_key(Keyboard::KEY::_F, is_pressed);
                break;
            }
            case GLFW_KEY_G: {
                keyboard.set_key(Keyboard::KEY::_G, is_pressed);
                break;
            }
            case GLFW_KEY_H: {
                keyboard.set_key(Keyboard::KEY::_H, is_pressed);
                break;
            }
            case GLFW_KEY_I: {
                keyboard.set_key(Keyboard::KEY::_I, is_pressed);
                break;
            }
            case GLFW_KEY_J: {
                keyboard.set_key(Keyboard::KEY::_J, is_pressed);
                break;
            }
            case GLFW_KEY_K: {
                keyboard.set_key(Keyboard::KEY::_K, is_pressed);
                break;
            }
            case GLFW_KEY_L: {
                keyboard.set_key(Keyboard::KEY::_L, is_pressed);
                break;
            }
            case GLFW_KEY_M: {
                keyboard.set_key(Keyboard::KEY::_M, is_pressed);
                break;
            }
            case GLFW_KEY_N: {
                keyboard.set_key(Keyboard::KEY::_N, is_pressed);
                break;
            }
            case GLFW_KEY_O: {
                keyboard.set_key(Keyboard::KEY::_O, is_pressed);
                break;
            }
            case GLFW_KEY_P: {
                keyboard.set_key(Keyboard::KEY::_P, is_pressed);
                break;
            }
            case GLFW_KEY_Q: {
                keyboard.set_key(Keyboard::KEY::_Q, is_pressed);
                break;
            }
            case GLFW_KEY_R: {
                keyboard.set_key(Keyboard::KEY::_R, is_pressed);
                break;
            }
            case GLFW_KEY_S: {
                keyboard.set_key(Keyboard::KEY::_S, is_pressed);
                break;
            }
            case GLFW_KEY_T: {
                keyboard.set_key(Keyboard::KEY::_T, is_pressed);
                break;
            }
            case GLFW_KEY_U: {
                keyboard.set_key(Keyboard::KEY::_U, is_pressed);
                break;
            }
            case GLFW_KEY_V: {
                keyboard.set_key(Keyboard::KEY::_V, is_pressed);
                break;
            }
            case GLFW_KEY_W: {
                keyboard.set_key(Keyboard::KEY::_W, is_pressed);
                break;
            }
            case GLFW_KEY_X: {
                keyboard.set_key(Keyboard::KEY::_X, is_pressed);
                break;
            }
            case GLFW_KEY_Y: {
                keyboard.set_key(Keyboard::KEY::_Y, is_pressed);
                break;
            }
            case GLFW_KEY_Z: {
                keyboard.set_key(Keyboard::KEY::_Z, is_pressed);
                break;
            }
            case GLFW_KEY_0: {
                keyboard.set_key(Keyboard::KEY::_0, is_pressed);
                break;
            }
            case GLFW_KEY_1: {
                keyboard.set_key(Keyboard::KEY::_1, is_pressed);
                break;
            }
            case GLFW_KEY_2: {
                keyboard.set_key(Keyboard::KEY::_2, is_pressed);
                break;
            }
            case GLFW_KEY_3: {
                keyboard.set_key(Keyboard::KEY::_3, is_pressed);
                break;
            }
            case GLFW_KEY_4: {
                keyboard.set_key(Keyboard::KEY::_4, is_pressed);
                break;
            }
            case GLFW_KEY_5: {
                keyboard.set_key(Keyboard::KEY::_5, is_pressed);
                break;
            }
            case GLFW_KEY_6: {
                keyboard.set_key(Keyboard::KEY::_6, is_pressed);
                break;
            }
            case GLFW_KEY_7: {
                keyboard.set_key(Keyboard::KEY::_7, is_pressed);
                break;
            }
            case GLFW_KEY_8: {
                keyboard.set_key(Keyboard::KEY::_8, is_pressed);
                break;
            }
            case GLFW_KEY_9: {
                keyboard.set_key(Keyboard::KEY::_9, is_pressed);
                break;
            }
            case GLFW_KEY_F1: {
                keyboard.set_key(Keyboard::KEY::_F1, is_pressed);
                break;
            }
            case GLFW_KEY_F2: {
                keyboard.set_key(Keyboard::KEY::_F2, is_pressed);
                break;
            }
            case GLFW_KEY_F3: {
                keyboard.set_key(Keyboard::KEY::_F3, is_pressed);
                break;
            }
            case GLFW_KEY_F4: {
                keyboard.set_key(Keyboard::KEY::_F4, is_pressed);
                break;
            }
            case GLFW_KEY_F5: {
                keyboard.set_key(Keyboard::KEY::_F5, is_pressed);
                break;
            }
            case GLFW_KEY_F6: {
                keyboard.set_key(Keyboard::KEY::_F6, is_pressed);
                break;
            }
            case GLFW_KEY_F7: {
                keyboard.set_key(Keyboard::KEY::_F7, is_pressed);
                break;
            }
            case GLFW_KEY_F8: {
                keyboard.set_key(Keyboard::KEY::_F8, is_pressed);
                break;
            }
            case GLFW_KEY_F9: {
                keyboard.set_key(Keyboard::KEY::_F9, is_pressed);
                break;
            }
            case GLFW_KEY_F10: {
                keyboard.set_key(Keyboard::KEY::_F10, is_pressed);
                break;
            }
            case GLFW_KEY_F11: {
                keyboard.set_key(Keyboard::KEY::_F11, is_pressed);
                break;
            }
            case GLFW_KEY_F12: {
                keyboard.set_key(Keyboard::KEY::_F12, is_pressed);
                break;
            }
            case GLFW_KEY_UP: {
                keyboard.set_key(Keyboard::KEY::_UP, is_pressed);
                break;
            }
            case GLFW_KEY_DOWN: {
                keyboard.set_key(Keyboard::KEY::_DOWN, is_pressed);
                break;
            }
            case GLFW_KEY_LEFT: {
                keyboard.set_key(Keyboard::KEY::_LEFT, is_pressed);
                break;
            }
            case GLFW_KEY_RIGHT: {
                keyboard.set_key(Keyboard::KEY::_RIGHT, is_pressed);
                break;
            }
            case GLFW_KEY_SPACE: {
                keyboard.set_key(Keyboard::KEY::_SPACE, is_pressed);
                break;
            }
            case GLFW_KEY_BACKSPACE: {
                keyboard.set_key(Keyboard::KEY::_BACKSPACE, is_pressed);
                break;
            }
            case GLFW_KEY_DELETE: {
                keyboard.set_key(Keyboard::KEY::_DELETE, is_pressed);
                break;
            }
            case GLFW_KEY_ENTER: {
                keyboard.enter = is_pressed;
                keyboard.set_key(Keyboard::KEY::_ENTER, is_pressed);
                break;
            }
            case GLFW_KEY_ESCAPE: {
                keyboard.esc = is_pressed;
                keyboard.set_key(Keyboard::KEY::_ESCAPE, is_pressed);
                break;
            }
            case GLFW_KEY_LEFT_SHIFT: {
                keyboard.shift = is_pressed;
                keyboard.set_key(Keyboard::KEY::_SHIFT, is_pressed);
                break;
            }
            case GLFW_KEY_RIGHT_SHIFT: {
                keyboard.shift = is_pressed;
                keyboard.set_key(Keyboard::KEY::_SHIFT, is_pressed);
                break;
            }
            case GLFW_KEY_LEFT_CONTROL: {
                keyboard.ctrl = is_pressed;
                keyboard.set_key(Keyboard::KEY::_CTRL, is_pressed);
                break;
            }
            case GLFW_KEY_RIGHT_CONTROL: {
                keyboard.ctrl = is_pressed;
                keyboard.set_key(Keyboard::KEY::_CTRL, is_pressed);
                break;
            }
            case GLFW_KEY_LEFT_ALT: {
                keyboard.alt = is_pressed;
                keyboard.set_key(Keyboard::KEY::_ALT, is_pressed);
                break;
            }
            case GLFW_KEY_RIGHT_ALT: {
                keyboard.alt = is_pressed;
                keyboard.set_key(Keyboard::KEY::_ALT, is_pressed);
                break;
            }
            case GLFW_KEY_TAB: {
                keyboard.set_key(Keyboard::KEY::_TAB, is_pressed);
                break;
            }
        }

        if (Engine::Instance()->key_callback) {
            Engine::Instance()->key_callback();
        }

        Engine::Dispatcher().trigger(Events::Update<Keyboard>{});
    }

    void SystemWindowGLFW::set_drop(void *window_handle, int count, const char **paths) {
        auto &input = Engine::Context().get<Input>();
        input.drop.paths.clear();
        for (int i = 0; i < count; ++i) {
            input.drop.paths.emplace_back(paths[i]);
        }

        if (Engine::Instance()->drop_callback) {
            Engine::Instance()->drop_callback();
        }

        Engine::Dispatcher().trigger(Events::Update<Input::Drop>{});
    }

    void SystemWindowGLFW::swap_and_poll_events() {
        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }

    void SystemWindowGLFW::pre_init() {
        Engine::Context().emplace<Window>();
        Engine::Context().emplace<Input>();
        Components<Window> windows(component_name());
        auto window_id = windows.create_instance();
        Engine::Context().emplace<Component<Window>>(window_id);
        Engine::Context().emplace<WindowRegister>();
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