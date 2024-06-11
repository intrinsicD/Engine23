//
// Created by alex on 08.05.24.
//

#include "CameraGui.h"
#include "Engine.h"
#include "imgui.h"
#include "SystemCamera.h"
#include "Components.h"
#include "Window.h"
#include "SystemWindowGLFW.h"

namespace Bcg{
    void ComponentGui<Camera<float>>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<Component<Camera<float>>>(entity_id)) {
            ComponentGui<Camera<float>>::Show(Engine::State().get<Component<Camera<float>>>(entity_id));
        }
    }

    void ComponentGui<Camera<float>>::Show(Component<Camera<float>> &component) {
        Components<Camera<float>> cameras(SystemCamera::component_name());
        return Show(cameras.get_instance(component));
    }

    void ComponentGui<Camera<float>>::Show(Camera<float> &camera) {
        auto model = camera.get_model();
        ImGui::Text("Model:");
        ImGui::Separator();
        ImGui::Text("%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n",
                    model(0, 0), model(0, 1), model(0, 2), model(0, 3),
                    model(1, 0), model(1, 1), model(1, 2), model(1, 3),
                    model(2, 0), model(2, 1), model(2, 2), model(2, 3),
                    model(3, 0), model(3, 1), model(3, 2), model(3, 3));

        auto view = camera.get_view();
        ImGui::Text("View:");
        ImGui::Separator();
        ImGui::Text("%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n",
                    view(0, 0), view(0, 1), view(0, 2), view(0, 3),
                    view(1, 0), view(1, 1), view(1, 2), view(1, 3),
                    view(2, 0), view(2, 1), view(2, 2), view(2, 3),
                    view(3, 0), view(3, 1), view(3, 2), view(3, 3));

        auto proj = camera.get_projection();
        ImGui::Text("Projection:");
        ImGui::Separator();
        ImGui::Text("%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n"
                    "%f %f %f %f\n",
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
            auto parameters = camera.view;
            auto position = parameters.position;
            auto front = parameters.get_front();
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
                    camera.view.set_position(position);
                }
                if (ImGui::InputFloat3("Front", front.data())) {
                    camera.view.set_front(front);
                }
                ImGui::Text("Right: %f %f %f", right[0], right[1], right[2]);
                if (ImGui::InputFloat3("World Up", up.data())) {
                    camera.view.set_worldup(up);
                }
            }
            if (ImGui::Button("Reset##View")) {
                camera.view.set_position(Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f));
                camera.view.set_worldup(Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f));
                camera.view.set_target(Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f));
            }
        }
        if (ImGui::CollapsingHeader("Projection Parameters")) {
            bool is_orthographic = camera.is_orthographic;
            if (is_orthographic) {
                OrthographicProjection orthographic = camera.projection.orthographic_parameters;
                if (!edit) {
                    ImGui::Text("Left: %f", orthographic.left);
                    ImGui::Text("Right: %f", orthographic.right);
                    ImGui::Text("Bottom: %f", orthographic.bottom);
                    ImGui::Text("Top: %f", orthographic.top);
                    ImGui::Text("Near##Orthographic: %f", orthographic.near);
                    ImGui::Text("Far##Orthographic: %f", orthographic.far);
                } else {
                    static bool changed_ortho = false;
                    static OrthographicProjection orthographic_new = orthographic;
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
                PerspectiveProjection perspective = camera.projection.perspective_parameters;
                if (!edit) {
                    ImGui::Text("Fovy: %f", perspective.fovy_degrees);
                    ImGui::Text("Aspect: %f", perspective.aspect);
                    ImGui::Text("Near##Orthographic: %f", perspective.near);
                    ImGui::Text("Far##Orthographic: %f", perspective.far);
                } else {
                    static bool changed_perspective = false;
                    static PerspectiveProjection perspective_new = perspective;
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
                    Components<Window> windows(SystemWindowGLFW::component_name());
                    auto &component_window = Engine::Context().get<Component<Window>>();
                    auto &window = windows.get_instance(component_window);
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
        static Eigen::Vector<float, 3> target = camera.view.target;
        if (ImGui::InputFloat3("target", target.data())) {
            camera.view.set_target(target);
        }

    }
}