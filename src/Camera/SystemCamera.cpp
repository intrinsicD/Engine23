//
// Created by alex on 15.12.23.
//

#include "SystemCamera.h"
#include "SystemsUtils.h"
#include "Engine.h"
#include "Events/Events.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "Window.h"
#include "Viewport.h"
#include "Input.h"
#include "Time.h"
#include "Camera.h"
#include "CameraGui.h"
#include "SafeAcos.h"
#include "Rotation3DAngleAxis.h"
#include "Components.h"
#include "SystemWindowGLFW.h"
#include "ImGuiUtils.h"
#include "Picker.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemCameraInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_viewport(const Events::Update<Viewport> &event);

        void on_update_input(const Events::Update<Input> &event);

        void on_update_mouse_button(const Events::Update<Input::Mouse::Button> &event);

        void on_update_mouse_position(const Events::Update<Input::Mouse::Position> &event);

        void on_update_mouse_scroll(const Events::Update<Input::Mouse::Scroll> &event);

        inline bool
        MapToSphere(const Eigen::Vector<float, 2> &point, int width, int height, Eigen::Vector<float, 3> &result);

        void on_end_main_loop(const Events::End<MainLoop> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemCameraInternal {
        void on_update_gui_instance(const Events::Update<Gui> &event) {
            if (!show_gui_instance) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemCamera::component_name().c_str(), &show_gui_instance)) {
                if (ImGui::CollapsingHeader("Main Camera")) {
                    auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
                    ComponentGui<Camera<float>>::Show(component_camera);
                }
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Camera<float>>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("CameraComponents", &show_gui_components)) {
                Components<Camera<float>> components(SystemCamera::component_name());
                ImGuiUtils::Show(components);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu(SystemCamera::component_name().c_str())) {
                    if (ImGui::MenuItem("Instance", nullptr, &show_gui_instance)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_instance>();
                    }
                    if (ImGui::MenuItem("Components", nullptr, &show_gui_components)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_components>();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_viewport(const Events::Update<Viewport> &event) {
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            auto &camera = cameras.get_instance(component_camera);
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);
            auto size = window.get_size();
            if (camera.is_orthographic) {
                camera.set_orthographic_parameters({-size[0] / 2.0f, size[0] / 2.0f,
                                                    -size[1] / 2.0f, size[1] / 2.0f,
                                                    camera.projection.orthographic_parameters.near,
                                                    camera.projection.orthographic_parameters.far});
            } else {
                camera.set_perspective_parameters({camera.projection.perspective_parameters.fovy_degrees,
                                                   window.get_aspect<float>(),
                                                   camera.projection.perspective_parameters.near,
                                                   camera.projection.perspective_parameters.far});
            }
        }

        void on_update_input(const Events::Update<Input> &event) {
            if (ImGui::GetIO().WantCaptureKeyboard) return;
            auto &keyboard = Engine::Context().get<Input>().keyboard;
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            auto &time = Engine::Context().get<Time>();

            auto delta = float(time.mainloop.duration) * camera.sensitivity.move;
            if (keyboard.keys[GLFW_KEY_W]) {
                //move forward
                camera.view.position += camera.view.front * delta;
            }
            if (keyboard.keys[GLFW_KEY_S]) {
                //move backward
                camera.view.position -= camera.view.front * delta;
            }
            if (keyboard.keys[GLFW_KEY_D]) {
                //move right
                camera.view.position += camera.view.right * delta;
            }
            if (keyboard.keys[GLFW_KEY_A]) {
                //move left
                camera.view.position -= camera.view.right * delta;
            }
        }

        void on_update_mouse_button(const Events::Update<Input::Mouse::Button> &event) {
            auto &input = Engine::Context().get<Input>();

            if (input.mouse.button.middle) {
                input.mouse.last_drag_pos = input.mouse.last_middle_click;
            }
        }

        void on_update_mouse_position(const Events::Update<Input::Mouse::Position> &event) {
            auto &input = Engine::Context().get<Input>();
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            if (input.mouse.button.middle) {
                Eigen::Vector<float, 2> pos_delta =
                        (input.mouse.position - input.mouse.last_drag_pos) * camera.sensitivity.drag;
                Eigen::Vector<float, 3> delta =
                        camera.view.up * pos_delta[1] - camera.view.right * pos_delta[0];
                camera.view.position += delta;
                camera.arc_ball_parameters.target += delta;

                input.mouse.last_drag_pos = input.mouse.position;
            }

            //TODO: implement arc ball camera controller
            //Rotate the camera around the target_point
            if (camera.arc_ball_parameters.last_point_ok && input.mouse.button.right) {
                auto model = camera.get_model();
                Eigen::Vector<float, 3> new_point_3d;
                Components<Window> windows(SystemWindowGLFW::component_name());
                auto &component_window = Engine::Context().get<Component<Window>>();
                auto &window = windows.get_instance(component_window);
                auto size = window.get_size();
                if (MapToSphere(input.mouse.position, size[0], size[1], new_point_3d)) {
                    if (new_point_3d[0] != camera.arc_ball_parameters.last_point_3d[0] ||
                        new_point_3d[1] != camera.arc_ball_parameters.last_point_3d[1] ||
                        new_point_3d[2] != camera.arc_ball_parameters.last_point_3d[2]) {
                        Eigen::Vector<float, 3> axis =
                                model.topLeftCorner<3, 3>() *
                                new_point_3d.cross(camera.arc_ball_parameters.last_point_3d).normalized();
                        float cos_angle = camera.arc_ball_parameters.last_point_3d.transpose() * new_point_3d;
                        float angle = SafeAcos(std::min(1.0f, cos_angle)) * camera.sensitivity.rotate;
                        Rotation3DAngleAxis<float> rot;
                        rot.m_angle_axis = angle * axis;

                        Eigen::Vector<float, 3> position = camera.view.position;
                        Eigen::Vector<float, 3> direction = position - camera.arc_ball_parameters.target;
                        direction = rot.matrix() * direction;
                        //if direction is nan, then dont update

                        camera.set_position(camera.arc_ball_parameters.target + direction);
                        camera.set_target(camera.arc_ball_parameters.target);
                    }
                }
            }

        }

        void on_update_mouse_scroll(const Events::Update<Input::Mouse::Scroll> &event) {
            if (ImGui::GetIO().WantCaptureMouse) return;

            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            auto &scroll = Engine::Context().get<Input>().mouse.scroll;
            auto &time = Engine::Context().get<Time>();

            if (camera.is_orthographic) {
                auto &orthographic_parameters = camera.projection.orthographic_parameters;
                auto delta = float(time.mainloop.duration) * camera.sensitivity.zoom;
                orthographic_parameters.top -= scroll[1] * delta;
                orthographic_parameters.bottom += scroll[1] * delta;
                orthographic_parameters.left -= scroll[0] * delta;
                orthographic_parameters.right += scroll[0] * delta;
            } else {
                auto &perspective_parameters = camera.projection.perspective_parameters;
                perspective_parameters.fovy_degrees -= scroll[1] * camera.sensitivity.zoom;
                if (perspective_parameters.fovy_degrees < 1.0f) {
                    perspective_parameters.fovy_degrees = 1.0f;
                } else if (perspective_parameters.fovy_degrees > 90.0f) {
                    perspective_parameters.fovy_degrees = 90.0f;
                }
            }
        }

        inline bool
        MapToSphere(const Eigen::Vector<float, 2> &point, int width, int height, Eigen::Vector<float, 3> &result) {
            if ((point[0] >= 0) && (point[0] <= float(width)) && (point[1] >= 0) &&
                (point[1] <= float(height))) {
                Eigen::Vector<float, 2> ndc({(point[0] / float(width) - 0.5), (0.5 - point[1] / float(height))});
                result[0] = std::sin(std::numbers::pi * ndc[0] * 0.5);
                result[1] = std::sin(std::numbers::pi * ndc[1] * 0.5);
                float sinx2siny2 = result[0] * result[0] + result[1] * result[1];
                result[2] = sinx2siny2 < 1.0 ? std::sqrt(1.0 - sinx2siny2) : 0.0;
                return true;
            }
            return false;
        }

        void on_end_main_loop(const Events::End<MainLoop> &event) {
            auto &input = Engine::Context().get<Input>();
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);
            auto size = window.get_size();
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            camera.arc_ball_parameters.last_point_2d = input.mouse.position;
            camera.arc_ball_parameters.last_point_ok = MapToSphere(camera.arc_ball_parameters.last_point_2d,
                                                                   size[0], size[1],
                                                                   camera.arc_ball_parameters.last_point_3d);
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<Input>>().connect<&on_update_input>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Scroll>>().connect<&on_update_mouse_scroll>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Position>>().connect<&on_update_mouse_position>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Button>>().connect<&on_update_mouse_button>();
            Engine::Dispatcher().sink<Events::End<MainLoop>>().connect<&on_end_main_loop>();
            Engine::Dispatcher().sink<Events::Update<Viewport>>().connect<&on_update_viewport>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&on_update_gui_menu>();
            Log::Info(SystemCamera::name(), "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<Input>>().disconnect<&on_update_input>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Scroll>>().disconnect<&on_update_mouse_scroll>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Position>>().disconnect<&on_update_mouse_position>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Button>>().disconnect<&on_update_mouse_button>();
            Engine::Dispatcher().sink<Events::Update<Viewport>>().disconnect<&on_update_viewport>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&on_update_gui_menu>();
            Log::Info(SystemCamera::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemCamera::name() {
        return "System" + component_name();
    }

    std::string SystemCamera::component_name() {
        return "Camera";
    }

    void SystemCamera::pre_init() {

    }

    void SystemCamera::init() {
        //register event handlers
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemCameraInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemCameraInternal::on_shutdown>();
        Components<Camera<float>> cameras(SystemCamera::component_name());
        auto camera_id = cameras.create_instance();
        auto &component_camera = Engine::Context().emplace<Component<Camera<float>>>(camera_id);
        auto &camera = cameras.get_instance(component_camera);
        Components<Window> windows(SystemWindowGLFW::component_name());
        auto &component_window = Engine::Context().get<Component<Window>>();
        auto &window = windows.get_instance(component_window);
        camera.set_perspective_parameters({45.0f, window.get_aspect<float>(), 0.1f, 100.0f});
        camera.set_position(Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f));
        camera.set_target(Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f));
        camera.set_worldup(Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f));
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemCamera::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine >>().disconnect<&SystemCameraInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine >>().disconnect<&SystemCameraInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}