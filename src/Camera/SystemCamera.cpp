//
// Created by alex on 15.12.23.
//

#include "SystemCamera.h"
#include "SystemsUtils.h"
#include "Engine.h"
#include "Commands.h"
#include "Events.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "components/Window.h"
#include "components/Viewport.h"
#include "components/Input.h"
#include "components/Time.h"
#include "Camera.h"
#include "SafeAcos.h"
#include "Eigen/Geometry"
#include "Rotation3DAngleAxis.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemCameraInternal {
        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

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
        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&on_render_gui>();
                return;
            }

            if (ImGui::Begin("Camera", &show_gui)) {
                auto &camera = Engine::Context().get<Camera>();
                auto model = camera.get_model();
                ImGui::Text("Model: %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n",
                            model(0, 0), model(0, 1), model(0, 2), model(0, 3),
                            model(1, 0), model(1, 1), model(1, 2), model(1, 3),
                            model(2, 0), model(2, 1), model(2, 2), model(2, 3),
                            model(3, 0), model(3, 1), model(3, 2), model(3, 3));

                auto view = camera.get_view();
                ImGui::Text("View: %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n",
                            view(0, 0), view(0, 1), view(0, 2), view(0, 3),
                            view(1, 0), view(1, 1), view(1, 2), view(1, 3),
                            view(2, 0), view(2, 1), view(2, 2), view(2, 3),
                            view(3, 0), view(3, 1), view(3, 2), view(3, 3));

                auto proj = camera.get_projection();
                ImGui::Text("Projection: %f %f %f %f\n"
                            "            %f %f %f %f\n"
                            "            %f %f %f %f\n"
                            "            %f %f %f %f\n",
                            proj(0, 0), proj(0, 1), proj(0, 2), proj(0, 3),
                            proj(1, 0), proj(1, 1), proj(1, 2), proj(1, 3),
                            proj(2, 0), proj(2, 1), proj(2, 2), proj(2, 3),
                            proj(3, 0), proj(3, 1), proj(3, 2), proj(3, 3));
                static bool edit = false;
                ImGui::InputFloat("Mov Speed", &camera.sensitivity.move);
                ImGui::InputFloat("Zoom Speed", &camera.sensitivity.zoom);
                ImGui::InputFloat("Rot Speed", &camera.sensitivity.rotate);
                ImGui::InputFloat("Drag Speed", &camera.sensitivity.drag);
                ImGui::Checkbox("Edit", &edit);

                if (ImGui::CollapsingHeader("View Parameters")) {
                    auto parameters = camera.view_parameters;
                    auto position = parameters.position;
                    auto front = parameters.front;
                    auto up = parameters.up;
                    auto world_up = parameters.world_up;
                    auto right = parameters.right;
                    if (!edit) {
                        ImGui::Text("Position: %f %f %f", position[0], position[1], position[2]);
                        ImGui::Text("Front: %f %f %f", front[0], front[1], front[2]);
                        ImGui::Text("Up: %f %f %f", up[0], up[1], up[2]);
                        ImGui::Text("Right: %f %f %f", right[0], right[1], right[2]);
                        ImGui::Text("World Up: %f %f %f", world_up[0], world_up[1], world_up[2]);
                    } else {
                        if (ImGui::InputFloat3("Position", position.data())) {
                            camera.set_position(position);
                        }
                        if (ImGui::InputFloat3("Front", front.data())) {
                            camera.set_front(front);
                        }
                        ImGui::Text("Right: %f %f %f", right[0], right[1], right[2]);
                        if (ImGui::InputFloat3("World Up", up.data())) {
                            camera.set_worldup(up);
                        }
                    }
                    if (ImGui::Button("Reset##View")) {
                        camera.set_position(Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f));
                        camera.set_worldup(Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f));
                        camera.set_target(Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f));
                    }
                }
                if (ImGui::CollapsingHeader("Projection Parameters")) {
                    bool is_orthographic = camera.is_orthographic;
                    if (is_orthographic) {
                        Camera::ProjectionParameters::Orthographic orthographic = camera.projection_parameters.orthographic_parameters;
                        if (!edit) {
                            ImGui::Text("Left: %f", orthographic.left);
                            ImGui::Text("Right: %f", orthographic.right);
                            ImGui::Text("Bottom: %f", orthographic.bottom);
                            ImGui::Text("Top: %f", orthographic.top);
                            ImGui::Text("Near##Orthographic: %f", orthographic.near);
                            ImGui::Text("Far##Orthographic: %f", orthographic.far);
                        } else {
                            static bool changed_ortho = false;
                            static Camera::ProjectionParameters::Orthographic orthographic_new = orthographic;
                            changed_ortho |= ImGui::InputFloat("Left", &orthographic_new.left);
                            changed_ortho |= ImGui::InputFloat("Right", &orthographic_new.right);
                            changed_ortho |= ImGui::InputFloat("Bottom", &orthographic_new.bottom);
                            changed_ortho |= ImGui::InputFloat("Top", &orthographic_new.top);
                            changed_ortho |= ImGui::InputFloat("Near##Orthographic", &orthographic_new.near);
                            changed_ortho |= ImGui::InputFloat("Far##Orthographic", &orthographic_new.far);
                            if (changed_ortho) {
                                if (ImGui::Button("Apply##Orthographic")) {
                                    changed_ortho = false;
                                    camera.set_orthographic_parameters(orthographic_new);
                                }
                            }
                        }
                        if (ImGui::Button("Reset##Orthographic")) {
                            camera.set_orthographic_parameters({-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f});
                        }
                    } else {
                        Camera::ProjectionParameters::Perspective perspective = camera.projection_parameters.perspective_parameters;
                        if (!edit) {
                            ImGui::Text("Fovy: %f", perspective.fovy_degrees);
                            ImGui::Text("Aspect: %f", perspective.aspect);
                            ImGui::Text("Near##Orthographic: %f", perspective.near);
                            ImGui::Text("Far##Orthographic: %f", perspective.far);
                        } else {
                            static bool changed_perspective = false;
                            static Camera::ProjectionParameters::Perspective perspective_new = perspective;
                            changed_perspective |= ImGui::InputFloat("Fovy", &perspective_new.fovy_degrees);
                            changed_perspective |= ImGui::InputFloat("Aspect", &perspective_new.aspect);
                            changed_perspective |= ImGui::InputFloat("Near##Perspective", &perspective_new.near);
                            changed_perspective |= ImGui::InputFloat("Far##Perspective", &perspective_new.far);
                            if (changed_perspective) {
                                if (ImGui::Button("Apply##Perspective")) {
                                    changed_perspective = false;
                                    camera.set_perspective_parameters(perspective_new);
                                }
                            }
                        }
                        if (ImGui::Button("Reset##Perspective")) {
                            auto &window = Engine::Context().get<Window>();
                            camera.set_perspective_parameters(
                                    {45.0f, window.get_aspect<float>(), 0.1f, 100.0f});
                        }
                    }
                }
                ImGui::Separator();

                ImGui::Text("last_point_ok: %d", camera.arc_ball_parameters.last_point_ok);
                ImGui::Text("last_point_2d: %f %f", camera.arc_ball_parameters.last_point_2d[0],
                            camera.arc_ball_parameters.last_point_2d[1]);
                ImGui::Text("last_point_3d: %f %f %f", camera.arc_ball_parameters.last_point_3d[0],
                            camera.arc_ball_parameters.last_point_3d[1],
                            camera.arc_ball_parameters.last_point_3d[2]);
                if (ImGui::InputFloat3("target", camera.arc_ball_parameters.target.data())) {
                    camera.set_target(camera.arc_ball_parameters.target);
                }

            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Camera", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_viewport(const Events::Update<Viewport> &event) {
            auto &camera = Engine::Context().get<Camera>();
            auto &window = Engine::Context().get<Window>();
            if (camera.is_orthographic) {
                camera.set_orthographic_parameters({-window.width / 2.0f, window.width / 2.0f,
                                                    -window.height / 2.0f, window.height / 2.0f,
                                                    camera.projection_parameters.orthographic_parameters.near,
                                                    camera.projection_parameters.orthographic_parameters.far});
            } else {
                camera.set_perspective_parameters({camera.projection_parameters.perspective_parameters.fovy_degrees,
                                                   window.get_aspect<float>(),
                                                   camera.projection_parameters.perspective_parameters.near,
                                                   camera.projection_parameters.perspective_parameters.far});
            }
        }

        void on_update_input(const Events::Update<Input> &event) {
            if (ImGui::GetIO().WantCaptureKeyboard) return;
            auto &keyboard = Engine::Context().get<Input>().keyboard;
            auto &camera = Engine::Context().get<Camera>();
            auto &time = Engine::Context().get<Time>();

            auto delta = float(time.mainloop.duration) * camera.sensitivity.move;
            if (keyboard.keys[GLFW_KEY_W]) {
                //move forward
                camera.view_parameters.position += camera.view_parameters.front * delta;
            }
            if (keyboard.keys[GLFW_KEY_S]) {
                //move backward
                camera.view_parameters.position -= camera.view_parameters.front * delta;
            }
            if (keyboard.keys[GLFW_KEY_D]) {
                //move right
                camera.view_parameters.position += camera.view_parameters.right * delta;
            }
            if (keyboard.keys[GLFW_KEY_A]) {
                //move left
                camera.view_parameters.position -= camera.view_parameters.right * delta;
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
            auto &camera = Engine::Context().get<Camera>();

            if (input.mouse.button.middle) {
                Eigen::Vector<float, 2> pos_delta = (input.mouse.position - input.mouse.last_drag_pos) * camera.sensitivity.drag;
                Eigen::Vector<float, 3> delta = camera.view_parameters.up * pos_delta[1] - camera.view_parameters.right * pos_delta[0];
                camera.view_parameters.position += delta;
                camera.arc_ball_parameters.target += delta;

                input.mouse.last_drag_pos = input.mouse.position;
            }

            //TODO: implement arc ball camera controller
            //Rotate the camera around the target_point
            if (camera.arc_ball_parameters.last_point_ok && input.mouse.button.right) {
                auto model = camera.get_model();
                Eigen::Vector<float, 3> new_point_3d;
                auto &window = Engine::Context().get<Window>();
                if (MapToSphere(input.mouse.position, window.width, window.height, new_point_3d)) {
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

                        Eigen::Vector<float, 3> position = camera.view_parameters.position;
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

            auto &camera = Engine::Context().get<Camera>();
            auto &scroll = Engine::Context().get<Input>().mouse.scroll;
            auto &time = Engine::Context().get<Time>();

            if (camera.is_orthographic) {
                auto &orthographic_parameters = camera.projection_parameters.orthographic_parameters;
                auto delta = float(time.mainloop.duration) * camera.sensitivity.zoom;
                orthographic_parameters.top -= scroll[1] * delta;
                orthographic_parameters.bottom += scroll[1] * delta;
                orthographic_parameters.left -= scroll[0] * delta;
                orthographic_parameters.right += scroll[0] * delta;
            } else {
                auto &perspective_parameters = camera.projection_parameters.perspective_parameters;
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
            auto &window = Engine::Context().get<Window>();
            auto &camera = Engine::Context().get<Camera>();
            camera.arc_ball_parameters.last_point_2d = input.mouse.position;
            camera.arc_ball_parameters.last_point_ok = MapToSphere(camera.arc_ball_parameters.last_point_2d,
                                                                   window.width,
                                                                   window.height,
                                                                   camera.arc_ball_parameters.last_point_3d);
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().connect<&on_update_input>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Scroll>>().connect<&on_update_mouse_scroll>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().connect<&on_update_mouse_position>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Button>>().connect<&on_update_mouse_button>();
            Engine::Instance()->dispatcher.sink<Events::End<MainLoop>>().connect<&on_end_main_loop>();
            Engine::Instance()->dispatcher.sink<Events::Update<Viewport>>().connect<&on_update_viewport>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&on_render_gui_menu>();
            Engine::State().on_construct<Camera>().connect<&on_construct_component<SystemCamera>>();
            Engine::State().on_update<Camera>().connect<&on_update_component<SystemCamera>>();
            Engine::State().on_destroy<Camera>().connect<&on_destroy_component<SystemCamera>>();
            Log::Info(SystemCamera::name(), "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().disconnect<&on_update_input>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Scroll>>().disconnect<&on_update_mouse_scroll>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().disconnect<&on_update_mouse_position>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Button>>().disconnect<&on_update_mouse_button>();
            Engine::Instance()->dispatcher.sink<Events::Update<Viewport>>().disconnect<&on_update_viewport>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&on_render_gui_menu>();
            Engine::State().on_construct<Camera>().disconnect<&on_construct_component<SystemCamera>>();
            Engine::State().on_update<Camera>().disconnect<&on_update_component<SystemCamera>>();
            Engine::State().on_destroy<Camera>().disconnect<&on_destroy_component<SystemCamera>>();
            Log::Info(SystemCamera::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemCamera::name() {
        return "SystemCamera";
    }

    std::string SystemCamera::component_name() {
        return "Camera";
    }

    void SystemCamera::pre_init() {
        //register necessary components
        auto &camera = Engine::Context().emplace<Camera>();
    }

    void SystemCamera::init() {
        //register event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemCameraInternal::on_shutdown>();
        auto &camera = Engine::Context().get<Camera>();
        auto &window = Engine::Context().get<Window>();
        camera.set_perspective_parameters({45.0f, window.get_aspect<float>(), 0.1f, 100.0f});
        camera.set_position(Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f));
        camera.set_target(Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f));
        camera.set_worldup(Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f));
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemCamera::remove() {
        Log::Info("Removed", name()).enqueue();
        //unregister event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<
                Engine >>().disconnect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<
                Engine >>().disconnect<&SystemCameraInternal::on_shutdown>();
    }
}