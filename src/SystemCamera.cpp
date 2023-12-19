//
// Created by alex on 15.12.23.
//

#include "SystemCamera.h"
#include "Engine.h"
#include "Components.h"
#include "Commands.h"
#include "Events.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace Bcg {
    namespace SystemCameraInternal {
        static bool show_gui = false;
        static bool is_first_person_camera = true;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&on_render_gui>();
                return;
            }

            if (ImGui::Begin("Camera", &show_gui)) {
                auto &camera = Engine::Context().get<Camera>();
                auto view = camera.get_view();
                ImGui::Text("View: %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n"
                            "      %f %f %f %f\n",
                            view[0][0], view[0][1], view[0][2], view[0][3],
                            view[1][0], view[1][1], view[1][2], view[1][3],
                            view[2][0], view[2][1], view[2][2], view[2][3],
                            view[3][0], view[3][1], view[3][2], view[3][3]);

                auto proj = camera.get_projection();
                ImGui::Text("Projection: %f %f %f %f\n"
                            "            %f %f %f %f\n"
                            "            %f %f %f %f\n"
                            "            %f %f %f %f\n",
                            proj[0][0], proj[0][1], proj[0][2], proj[0][3],
                            proj[1][0], proj[1][1], proj[1][2], proj[1][3],
                            proj[2][0], proj[2][1], proj[2][2], proj[2][3],
                            proj[3][0], proj[3][1], proj[3][2], proj[3][3]);
                static bool edit = false;
                ImGui::InputFloat("Mov Speed", &camera.mov_speed);
                ImGui::InputFloat("Zoom Speed", &camera.zoom_speed);
                ImGui::InputFloat("Rot Speed", &camera.rot_speed);
                ImGui::Checkbox("Edit", &edit);
                int i_is_first_person_camera = is_first_person_camera;
                if (ImGui::RadioButton("ArcBall", &i_is_first_person_camera, 0)) {
                    SystemCamera::make_arc_ball_camera();
                }
                if (ImGui::RadioButton("Fps", &i_is_first_person_camera, 1)) {
                    SystemCamera::make_first_person_camera();
                }
                if (ImGui::CollapsingHeader("View Parameters")) {
                    auto position = camera.get_position();
                    auto direction = camera.get_direction();
                    auto up = camera.get_up();
                    auto right = camera.get_right();
                    auto forward = camera.get_forward();
                    if (!edit) {
                        ImGui::Text("Position: %f %f %f", position.x, position.y, position.z);
                        ImGui::Text("Direction: %f %f %f", direction.x, direction.y, direction.z);
                        ImGui::Text("Forward: %f %f %f", forward.x, forward.y, forward.z);
                        ImGui::Text("Up: %f %f %f", up.x, up.y, up.z);
                    } else {
                        if (ImGui::InputFloat3("Position", &position.x)) {
                            camera.set(position, position + forward, up);
                        }
                        if (ImGui::InputFloat3("Direction", &direction.x)) {
                            camera.set(position, position - direction, up);
                        }
                        if (ImGui::InputFloat3("Forward", &forward.x)) {
                            camera.set(position, position + forward, up);
                        }
                        if (ImGui::InputFloat3("Up", &up.x)) {
                            camera.set(position, position + forward, up);
                        }
                    }
                    ImGui::Text("Right: %f %f %f", right.x, right.y, right.z);
                    if (ImGui::Button("Reset##View")) {
                        camera.set(glm::vec3(0.0f, 0.0f, 3.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
                    }
                }
                if (ImGui::CollapsingHeader("Projection Parameters")) {
                    bool is_orthographic = camera.is_orthographic;
                    if (is_orthographic) {
                        Camera::ProjectionParameters::Orthographic orthographic = camera.get_orthographic_parameters();
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
                        Camera::ProjectionParameters::Perspective perspective = camera.get_perspective_parameters();
                        if (!edit) {
                            ImGui::Text("Fovy: %f", perspective.fovy);
                            ImGui::Text("Aspect: %f", perspective.aspect);
                            ImGui::Text("Near##Orthographic: %f", perspective.near);
                            ImGui::Text("Far##Orthographic: %f", perspective.far);
                        } else {
                            static bool changed_perspective = false;
                            static Camera::ProjectionParameters::Perspective perspective_new = perspective;
                            changed_perspective |= ImGui::InputFloat("Fovy", &perspective_new.fovy);
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
                            auto &window_config = Engine::Context().get<WindowConfig>();
                            camera.set_perspective_parameters(
                                    {45.0f, window_config.get_aspect<float>(), 0.1f, 100.0f});
                        }
                    }
                }
                ImGui::Separator();
                if (i_is_first_person_camera) {
                    auto &fps = Engine::Context().get<FPSCameraController>();
                    ImGui::Text("Yaw: %f", fps.yaw);
                    ImGui::Text("Pitch: %f", fps.pitch);
                    ImGui::InputFloat("Sensitivity", &fps.sensitivity);
                } else {
                    auto &arc_ball = Engine::Context().get<ArcBallCameraController>();
                    ImGui::Text("last_point_ok: %d", arc_ball.last_point_ok);
                    ImGui::Text("last_point_2d: %f %f", arc_ball.last_point_2d.x, arc_ball.last_point_2d.y);
                    ImGui::Text("last_point_3d: %f %f %f", arc_ball.last_point_3d.x, arc_ball.last_point_3d.y,
                                arc_ball.last_point_3d.z);
                    if (ImGui::InputFloat3("target", &arc_ball.target.x)) {
                        arc_ball.camera.set(arc_ball.camera.get_position(), arc_ball.target, arc_ball.camera.get_up());
                    }
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
            auto &window_config = Engine::Context().get<WindowConfig>();
            if (camera.is_orthographic) {
                camera.set_orthographic_parameters({-window_config.width / 2.0f, window_config.width / 2.0f,
                                                    -window_config.height / 2.0f, window_config.height / 2.0f,
                                                    camera.get_orthographic_parameters().near,
                                                    camera.get_orthographic_parameters().far});
            } else {
                camera.set_perspective_parameters({camera.get_perspective_parameters().fovy,
                                                   window_config.get_aspect<float>(),
                                                   camera.get_perspective_parameters().near,
                                                   camera.get_perspective_parameters().far});
            }
        }

        void on_update_input(const Events::Update<Input> &event) {
            auto &keyboard = Engine::Context().get<Input>().keyboard;
            auto &camera = Engine::Context().get<Camera>();
            auto &time = Engine::Context().get<Time>();

            auto &model = camera.model;
            auto delta = float(time.mainloop.duration) * camera.mov_speed;
            bool changed = false;
            if (keyboard.keys[GLFW_KEY_W]) {
                //move forward
                model = glm::translate(model, camera.get_forward() * delta);
                changed = true;
            }
            if (keyboard.keys[GLFW_KEY_S]) {
                //move backward
                model = glm::translate(model, -camera.get_forward() * delta);
                changed = true;
            }

            auto right = camera.get_right();
            if (keyboard.keys[GLFW_KEY_D]) {
                //move right
                model = glm::translate(model, right * delta);
                changed = true;
            }
            if (keyboard.keys[GLFW_KEY_A]) {
                //move left
                model = glm::translate(model, -right * delta);
                changed = true;
            }
            if (changed) {
                camera.set_model(model);
            }
        }

        void on_update_mouse_button(const Events::Update<Input::Mouse::Button> &event) {
            auto &input = Engine::Context().get<Input>();

            if (input.mouse.button.middle) {
                auto &camera = Engine::Context().get<Camera>();
                auto &window_config = Engine::Context().get<WindowConfig>();
                input.mouse.drag_begin = glm::unProject(glm::vec3(input.mouse.last_middle_click, 0.0f),
                                                        camera.get_view(), camera.get_projection(),
                                                        glm::vec4(0, 0, window_config.width, window_config.height));
            }
        }

        void on_update_mouse_position(const Events::Update<Input::Mouse::Position> &event) {
            auto &input = Engine::Context().get<Input>();

            if (input.mouse.button.middle) {
                auto &camera = Engine::Context().get<Camera>();
                auto &window_config = Engine::Context().get<WindowConfig>();
                auto &time = Engine::Context().get<Time>();
                auto delta = float(time.mainloop.duration) * camera.mov_speed;
                auto right = camera.get_right();
                auto up = camera.get_up();
                auto old_position = camera.get_position();
                float xoffset = input.mouse.position.x - input.mouse.drag_begin.x;
                float yoffset = input.mouse.drag_begin.y -
                                input.mouse.position.y; // reversed since y-coordinates go from bottom to top
                auto offset = glm::unProject(glm::vec3(xoffset, yoffset, 0.0f),
                                             camera.get_view(), camera.get_projection(),
                                             glm::vec4(0, 0, window_config.width, window_config.height));
                auto new_position = old_position + offset * camera.mov_speed;
                new_position = old_position;
                new_position -= right * delta * input.mouse.position_delta.x;
                new_position += up * delta * input.mouse.position_delta.y;
                camera.set(new_position, new_position + camera.get_forward(), up);
            }
        }

        void on_update_mouse_scroll(const Events::Update<Input::Mouse::Scroll> &event) {
            auto &camera = Engine::Context().get<Camera>();
            auto &scroll = Engine::Context().get<Input>().mouse.scroll;
            auto &time = Engine::Context().get<Time>();

            if (camera.is_orthographic) {
                auto orthographic_parameters = camera.get_orthographic_parameters();
                auto delta = float(time.mainloop.duration) * camera.zoom_speed;
                orthographic_parameters.top -= scroll.y * delta;
                orthographic_parameters.bottom += scroll.y * delta;
                orthographic_parameters.left -= scroll.x * delta;
                orthographic_parameters.right += scroll.x * delta;
                camera.set_orthographic_parameters(orthographic_parameters);
            } else {
                auto perspective_parameters = camera.get_perspective_parameters();
                auto delta = float(time.mainloop.duration) * camera.zoom_speed;
                perspective_parameters.fovy -= scroll.y * delta;
                camera.set_perspective_parameters(perspective_parameters);
            }
        }

        void on_update_fps_controller(const Events::Update<Input::Mouse::Position> &event) {
            auto &fps = Engine::Context().get<FPSCameraController>();
            auto &input = Engine::Context().get<Input>();

            if (!input.mouse.button.right) {
                return;
            }

            float xpos = input.mouse.position.x;
            float ypos = input.mouse.position.y;

            if (fps.first_mouse) {
                fps.last_x = xpos;
                fps.last_y = ypos;
                fps.first_mouse = false;
            }

            float xoffset = xpos - fps.last_x;
            float yoffset = fps.last_y - ypos; // reversed since y-coordinates go from bottom to top
            fps.last_x = xpos;
            fps.last_y = ypos;

            xoffset *= fps.sensitivity;
            yoffset *= fps.sensitivity;

            fps.yaw += xoffset;
            fps.pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (fps.pitch > 89.0f)
                fps.pitch = 89.0f;
            if (fps.pitch < -89.0f)
                fps.pitch = -89.0f;

            glm::vec3 forward;
            forward.x = cos(glm::radians(fps.yaw)) * cos(glm::radians(fps.pitch));
            forward.y = sin(glm::radians(fps.pitch));
            forward.z = sin(glm::radians(fps.yaw)) * cos(glm::radians(fps.pitch));
            auto position = fps.camera.get_position();
            fps.camera.set(position, position + glm::normalize(forward), fps.camera.get_up());
        }

        inline bool
        MapToSphere(const glm::vec2 &point, int width, int height, glm::vec3 &result) {
            if ((point[0] >= 0) && (point[0] <= float(width)) && (point[1] >= 0) &&
                (point[1] <= float(height))) {
                glm::vec2 ndc({(point[0] / float(width) - 0.5), (0.5 - point[1] / float(height))});
                result[0] = std::sin(std::numbers::pi * ndc[0] * 0.5);
                result[1] = std::sin(std::numbers::pi * ndc[1] * 0.5);
                float sinx2siny2 = result[0] * result[0] + result[1] * result[1];
                result[2] = sinx2siny2 < 1.0 ? std::sqrt(1.0 - sinx2siny2) : 0.0;
                return true;
            }
            return false;
        }

        double SafeAcos(double value) {
            if (value >= 1.0) {
                return 0;
            } else if (value <= -1.0) {
                return std::numbers::pi;
            } else {
                return std::acos(value);
            }
        }


        void on_end_main_loop(const Events::End<MainLoop> &event) {
            auto &arc_ball = Engine::Context().get<ArcBallCameraController>();
            auto &input = Engine::Context().get<Input>();
            auto &window_config = Engine::Context().get<WindowConfig>();
            arc_ball.last_point_2d = input.mouse.position;
            arc_ball.last_point_ok = MapToSphere(arc_ball.last_point_2d,
                                                 window_config.width,
                                                 window_config.height,
                                                 arc_ball.last_point_3d);
        }

        void on_update_arc_ball_controller(const Events::Update<Input::Mouse::Position> &event) {
            auto &arc_ball = Engine::Context().get<ArcBallCameraController>();
            auto &input = Engine::Context().get<Input>();
            auto &window_config = Engine::Context().get<WindowConfig>();

            //TODO: implement arc ball camera controller
            //Rotate the camera around the target_point
            if (arc_ball.last_point_ok && input.mouse.button.right) {
                auto &model = arc_ball.camera.model;
                glm::vec3 new_point_3d;
                if (MapToSphere(input.mouse.position, window_config.width, window_config.height, new_point_3d)) {
                    glm::vec3 axis =
                            glm::mat3(model) * glm::normalize(glm::cross(new_point_3d, arc_ball.last_point_3d));
                    float cos_angle = glm::dot(arc_ball.last_point_3d, new_point_3d);
                    float angle = SafeAcos(cos_angle) * arc_ball.camera.rot_speed;

                    auto position = arc_ball.camera.get_position();
                    auto direction = position - arc_ball.target;
                    auto rot = glm::rotate(glm::mat4(1.0f), angle, axis);
                    direction = glm::vec3(rot * glm::vec4(direction, 0.0f));
                    position = arc_ball.target + direction;
                    arc_ball.camera.set(position, arc_ball.target, arc_ball.camera.get_up());
                }
            }
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().connect<&on_update_input>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Scroll>>().connect<&on_update_mouse_scroll>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().connect<&on_update_mouse_position>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Button>>().connect<&on_update_mouse_button>();
            Engine::Instance()->dispatcher.sink<Events::End<MainLoop>>().connect<&on_end_main_loop>();
            Engine::Instance()->dispatcher.sink<Events::Update<Viewport>>().connect<&on_update_viewport>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&on_render_gui_menu>();
            SystemCamera::make_arc_ball_camera();
            Log::Info(SystemCamera::name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().disconnect<&on_update_input>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Scroll>>().disconnect<&on_update_mouse_scroll>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().disconnect<&on_update_mouse_position>();
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Button>>().disconnect<&on_update_mouse_button>();
            Engine::Instance()->dispatcher.sink<Events::Update<Viewport>>().disconnect<&on_update_viewport>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&on_render_gui_menu>();
            Log::Info(SystemCamera::name() + ": Shutdown").enqueue();
        }
    }

    std::string SystemCamera::name() {
        return "camera";
    }


    void SystemCamera::make_arc_ball_camera() {
        SystemCameraInternal::is_first_person_camera = false;
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().connect<&SystemCameraInternal::on_update_arc_ball_controller>();
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().disconnect<&SystemCameraInternal::on_update_fps_controller>();
    }

    void SystemCamera::make_first_person_camera() {
        SystemCameraInternal::is_first_person_camera = true;
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().connect<&SystemCameraInternal::on_update_fps_controller>();
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Mouse::Position>>().disconnect<&SystemCameraInternal::on_update_arc_ball_controller>();
    }

    void SystemCamera::toggle_camera_mode() {
        if (SystemCameraInternal::is_first_person_camera) {
            make_arc_ball_camera();
        } else {
            make_first_person_camera();
        }
    }

    void SystemCamera::pre_init() {
        //register necessary components
        auto &camera = Engine::Context().emplace<Camera>();
        Engine::Context().emplace<ArcBallCameraController>(camera);
        Engine::Context().emplace<FPSCameraController>(camera);
    }

    void SystemCamera::init() {
        //register event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemCameraInternal::on_shutdown>();
        auto &camera = Engine::Context().get<Camera>();
        auto &window_config = Engine::Context().get<StartupWindowConfig>();
        camera.set_perspective_parameters({45.0f, window_config.get_aspect<float>(), 0.1f, 100.0f});
        camera.set(glm::vec3(0.0f, 0.0f, 3.0f),
                   glm::vec3(0.0f, 0.0f, 3.0f) + glm::vec3(0.0f, 0.0f, -1.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f));

        auto &fps = Engine::Context().get<FPSCameraController>();
        fps.last_x = window_config.width / 2.0f;
        fps.last_y = window_config.height / 2.0f;
    }

    void SystemCamera::remove() {
        //unregister event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<
                Engine >>().disconnect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<
                Engine >>().disconnect<&SystemCameraInternal::on_shutdown>();
    }
}