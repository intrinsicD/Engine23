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
#include "Time.h"
#include "Camera.h"
#include "CameraGui.h"
#include "SafeAcos.h"
#include "Rotation3DAngleAxis.h"
#include "Components.h"
#include "SystemWindowGLFW.h"
#include "ImGuiUtils.h"
#include "Picker.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "SystemRendererOpenGL.h"
#include <iostream>

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

        void on_update_keyboard(const Events::Update<Keyboard> &event);

        void on_update_mouse_button(const Events::Update<Mouse<float>::Button> &event);

        void on_update_mouse_position(const Events::Update<Mouse<float>::Position> &event);

        void on_update_mouse_scroll(const Events::Update<Mouse<float>::Scroll> &event);

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

        void on_update_keyboard(const Events::Update<Keyboard> &event) {
            if (ImGui::GetIO().WantCaptureKeyboard) return;
            auto &keyboard = Engine::Context().get<Keyboard>();
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

        void on_update_mouse_button(const Events::Update<Mouse<float>::Button> &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();

            if (mouse.button.middle) {
                mouse.position.last_drag_pos = mouse.position.last_middle_click;
            }
        }


        float GetDepthValueAtScreenPosition(const Eigen::Vector<float, 2> &screen_pos) {
            // Assuming you have a way to get the depth value from the depth buffer
            // This is a placeholder function and should be replaced with actual depth buffer access
            return SystemRendererOpenGL::get_depth_at_screen_position(screen_pos[0], screen_pos[1]);
        }

        // Helper function to convert screen coordinates to world coordinates
        void ScreenToWorld(const Eigen::Vector<float, 2> &screen_pos, Camera<float> &camera,
                           Eigen::Vector<float, 3> &world_pos) {
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);
            auto size = window.get_size();
            //clip screenpos at window borders:
            Eigen::Vector<float, 2> clipped_pos = screen_pos;
            clipped_pos[0] = std::max(0.0f, std::min(clipped_pos[0], float(size[0])));
            clipped_pos[1] = std::max(0.0f, std::min(clipped_pos[1], float(size[1])));

            // Get the depth value at the screen position (assuming a depth buffer is available)
            float depth = GetDepthValueAtScreenPosition(clipped_pos);

            Eigen::Vector<float, 2> ndc = WindowCoordsToNormalizedDeviceCoordinates(clipped_pos, size[0], size[1]);
            Eigen::Vector<float, 3> view = NormalizedDeviceCoordinatesToViewCoords(ndc, depth, camera.get_projection());
            world_pos = ViewCoordsToWorldCoords(view, camera.get_view());
            //TODO fix camera when dragging and rotating

            /*        float x = (2.0f * screen_pos[0]) / size[0] - 1.0f;
                    float y = 1.0f - (2.0f * screen_pos[1]) / size[1];
                    float z = depth; // Use the actual depth value

                    Eigen::Vector<float, 4> screen_space_pos(x, y, z, 1.0f);
                    Eigen::Vector<float, 4> world_space_pos = camera.get_projection().inverse() * screen_space_pos;
                    world_space_pos /= world_space_pos.w();

                    world_pos = world_space_pos.head<3>();*/
        }

        void on_update_mouse_position(const Events::Update<Mouse<float>::Position> &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);

            if (mouse.button.middle) {
                auto &picker = Engine::Context().get<Picker>();
                if (picker.id.entity != entt::null) {

                }
                //------------------------------------------------------------------------------------------------------

                Components<Window> windows(SystemWindowGLFW::component_name());
                auto &component_window = Engine::Context().get<Component<Window>>();
                auto &window = windows.get_instance(component_window);
                auto size = window.get_size();

                if (!(mouse.position.current[0] < 0.0f || mouse.position.current[0] > size[0] ||
                      mouse.position.current[1] < 0.0f || mouse.position.current[1] > size[1])) {
                    Eigen::Vector<float, 3> start;
                    ScreenToWorld(mouse.position.last_drag_pos, camera, start);
                    Eigen::Vector<float, 3> end;
                    ScreenToWorld(mouse.position.current, camera, end);
                    Eigen::Vector<float, 3> world_delta = end - start;
                    world_delta[2] = 0;
                    if (!picker.point.is_background) {
                        camera.view.position -= world_delta;
                        camera.arc_ball_parameters.target -= world_delta;
                        mouse.position.last_drag_pos = mouse.position.current;
                    }
                }
            }

            // Handle other mouse events, e.g., right button for arc ball camera controller
            if (camera.arc_ball_parameters.last_point_ok && mouse.button.right) {
                auto model = camera.get_model();
                Eigen::Vector<float, 3> new_point_3d;
                Components<Window> windows(SystemWindowGLFW::component_name());
                auto &component_window = Engine::Context().get<Component<Window>>();
                auto &window = windows.get_instance(component_window);
                auto size = window.get_size();
                if (MapToSphere(mouse.position.current, size[0], size[1], new_point_3d)) {
                    if (new_point_3d != camera.arc_ball_parameters.last_point_3d) {
                        Eigen::Vector<float, 3> axis = model.topLeftCorner<3, 3>() *
                                                       new_point_3d.cross(
                                                               camera.arc_ball_parameters.last_point_3d).normalized();
                        float cos_angle = camera.arc_ball_parameters.last_point_3d.dot(new_point_3d);
                        float angle = SafeAcos(std::min(1.0f, cos_angle)) * camera.sensitivity.rotate;
                        Rotation3DAngleAxis<float> rot;
                        rot.m_angle_axis = angle * axis;

                        Eigen::Vector<float, 3> position = camera.view.position;
                        Eigen::Vector<float, 3> direction = position - camera.arc_ball_parameters.target;
                        direction = rot.matrix() * direction;


                        camera.set_position(camera.arc_ball_parameters.target + direction);
                        camera.set_target(camera.arc_ball_parameters.target);

                    }
                }
            }
        }

        void on_update_mouse_scroll(const Events::Update<Mouse<float>::Scroll> &event) {
            if (ImGui::GetIO().WantCaptureMouse) return;

            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            auto &scroll = Engine::Context().get<Mouse<float>>().scroll;
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
            auto &mouse = Engine::Context().get<Mouse<float>>();
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);
            auto size = window.get_size();
            auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
            Components<Camera<float>> cameras(SystemCamera::component_name());
            auto &camera = cameras.get_instance(component_camera);
            camera.arc_ball_parameters.last_point_2d = mouse.position.current;
            camera.arc_ball_parameters.last_point_ok = MapToSphere(camera.arc_ball_parameters.last_point_2d,
                                                                   size[0], size[1],
                                                                   camera.arc_ball_parameters.last_point_3d);
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<Keyboard>>().connect<&on_update_keyboard>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Scroll>>().connect<&on_update_mouse_scroll>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Position>>().connect<&on_update_mouse_position>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Button>>().connect<&on_update_mouse_button>();
            Engine::Dispatcher().sink<Events::End<MainLoop>>().connect<&on_end_main_loop>();
            Engine::Dispatcher().sink<Events::Update<Viewport>>().connect<&on_update_viewport>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&on_update_gui_menu>();
            Log::Info(SystemCamera::name(), "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<Keyboard>>().disconnect<&on_update_keyboard>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Scroll>>().disconnect<&on_update_mouse_scroll>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Position>>().disconnect<&on_update_mouse_position>();
            Engine::Dispatcher().sink<Events::Update<Mouse<float>::Button>>().disconnect<&on_update_mouse_button>();
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