//
// Created by alex on 26.04.24.
//

#include "SystemMouse.h"
#include "Engine.h"
#include "Events/Events.h"
#include "InternalCallbackEvents.h"
#include "Mouse.h"
#include "Commands.h"
#include "imgui.h"
#include "GLFW/glfw3.h"
#include "TypeName.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {


    namespace SystemMouseInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_end_main_loop(const Events::End<MainLoop> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_callback_mouse_button(const Events::Callback::MouseButton &event);

        void on_callback_mouse_cursor_position(const Events::Callback::MouseCursorPosition &event);

        void on_callback_mouse_scroll(const Events::Callback::MouseScroll &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMouseInternal {

        void RenderGuiMouseState(Mouse<float>::State &state) {
            std::vector<std::string> state_string = {"Idle", "Move", "Drag", "Scroll"};
            ImGui::Text("State: %s: %s", state_string[state].c_str(), std::to_string(static_cast<int>(state)).c_str());
        }

        void RenderGuiMouseButton(Mouse<float>::Button &button) {
            std::string button_string = (button.left ? "left" : (button.middle ? "middle"
                                                                               : (button.right
                                                                                  ? "right" : "None")));
            ImGui::Text("Button: (%d, %d, %d) %s", button.left, button.middle, button.right, button_string.c_str());
        }

        void RenderGuiMousePosition(Mouse<float>::Position &position) {
            ImGui::Text("Current:          (%f %f)", position.current[0], position.current[1]);
            ImGui::Text("Delta:            (%f %f)", position.delta[0], position.delta[1]);
            ImGui::Text("LastLeftClick:    (%f %f)", position.last_left_click[0], position.last_left_click[1]);
            ImGui::Text("LastMiddleClick:  (%f %f)", position.last_middle_click[0], position.last_middle_click[1]);
            ImGui::Text("LastRightClick:   (%f %f)", position.last_right_click[0], position.last_right_click[1]);
        }

        void RenderGuiMouseScroll(Eigen::Vector<float, 2> &scroll) {
            ImGui::Text("Scroll:          (%f %f)", scroll[0], scroll[1]);
        }

        void RenderGuiMouse(Mouse<float> &mouse) {
            RenderGuiMouseState(mouse.state);
            RenderGuiMouseButton(mouse.button);
            RenderGuiMousePosition(mouse.position);
            RenderGuiMouseScroll(mouse.scroll);
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }


            if (ImGui::Begin("Mouse", &show_gui)) {
                auto &mouse = Engine::Context().get<Mouse<float>>();
                RenderGuiMouse(mouse);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu("Input")) {
                    if (ImGui::MenuItem("Mouse", nullptr, &show_gui)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
        }

        void on_end_main_loop(const Events::End<MainLoop> &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            mouse.state = Mouse<float>::State::IDLE;
            mouse.scroll.setZero();
            mouse.gui_capture = false;
        }

        template<typename T>
        void SetPosition(void *window_ptr, Eigen::Vector<T, 2> &pos) {
            double xpos, ypos;
            glfwGetCursorPos(static_cast<GLFWwindow *>(window_ptr), &xpos, &ypos);
            pos[0] = T(xpos);
            pos[1] = T(ypos);
        }

        void on_callback_mouse_button(const Events::Callback::MouseButton &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
                mouse.button.left = event.action == GLFW_PRESS;
                if (event.action == GLFW_PRESS) {
                    SetPosition(event.window_ptr, mouse.position.last_left_click);
                }
            } else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
                mouse.button.right = event.action == GLFW_PRESS;
                if (event.action == GLFW_PRESS) {
                    SetPosition(event.window_ptr, mouse.position.last_right_click);
                }
            } else if (event.button == GLFW_MOUSE_BUTTON_MIDDLE) {
                mouse.button.middle = event.action == GLFW_PRESS;
                if (event.action == GLFW_PRESS) {
                    SetPosition(event.window_ptr, mouse.position.last_middle_click);
                }
            }
        }

        void on_callback_mouse_cursor_position(const Events::Callback::MouseCursorPosition &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            Eigen::Vector<float, 2> current(event.x, event.y);
            mouse.position.delta = current - mouse.position.current;
            mouse.position.current = current;
        }

        void on_callback_mouse_scroll(const Events::Callback::MouseScroll &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            mouse.scroll = {event.x, event.y};
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemMouseInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::End<MainLoop>>().connect<&SystemMouseInternal::on_end_main_loop>();
            Engine::Dispatcher().sink<Events::Callback::MouseButton>().connect<&SystemMouseInternal::on_callback_mouse_button>();
            Engine::Dispatcher().sink<Events::Callback::MouseCursorPosition>().connect<&SystemMouseInternal::on_callback_mouse_cursor_position>();
            Engine::Dispatcher().sink<Events::Callback::MouseScroll>().connect<&SystemMouseInternal::on_callback_mouse_scroll>();

            Log::Info("Startup", SystemMouse::name()).enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemMouseInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::End<MainLoop>>().disconnect<&SystemMouseInternal::on_end_main_loop>();
            Engine::Dispatcher().sink<Events::Callback::MouseButton>().disconnect<&SystemMouseInternal::on_callback_mouse_button>();
            Engine::Dispatcher().sink<Events::Callback::MouseCursorPosition>().disconnect<&SystemMouseInternal::on_callback_mouse_cursor_position>();
            Engine::Dispatcher().sink<Events::Callback::MouseScroll>().disconnect<&SystemMouseInternal::on_callback_mouse_scroll>();

            Log::Info("Shutdown", SystemMouse::name()).enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {

    std::string SystemMouse::name() {
        return "System" + component_name();
    }

    std::string SystemMouse::component_name() {
        return TypeName<Mouse<float>>::name();
    }

    void SystemMouse::pre_init() {
        Engine::Context().emplace<Mouse<float>>();
    }

    void SystemMouse::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemMouseInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemMouseInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemMouse::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemMouseInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemMouseInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}