//
// Created by alex on 26.02.24.
//

#include "TransformGui.h"
#include "Transform.h"
#include "Engine.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "Component.h"
#include "ResourceContainer.h"

namespace Bcg {
    static bool show_guizmo = false;

    inline void EditTransform(const Camera<float> &camera, Eigen::Matrix<float, 4, 4> &matrix) {
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE)) {
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE)) {
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        }
        Eigen::Vector<float, 3> translation, rotation, scale;
        ImGuizmo::DecomposeMatrixToComponents(matrix.data(),
                                              translation.data(),
                                              rotation.data(),
                                              scale.data());
        ImGui::InputFloat3("Tr", translation.data());
        ImGui::InputFloat3("Rt", rotation.data());
        ImGui::InputFloat3("Sc", scale.data());
        ImGuizmo::RecomposeMatrixFromComponents(translation.data(),
                                                rotation.data(),
                                                scale.data(),
                                                matrix.data());

        if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        static bool useSnap(false);
        ImGui::Checkbox("use_snap", &useSnap);
        ImGui::SameLine();
        static Eigen::Vector<float, 3> snap = Eigen::Vector<float, 3>::Ones();
        switch (mCurrentGizmoOperation) {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", snap.data());
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", snap.data());
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", snap.data());
                break;
            default:
                break;
        }
        ImGuiIO &io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::Manipulate(camera.get_view().data(), camera.get_projection().data(),
                             mCurrentGizmoOperation, mCurrentGizmoMode,
                             matrix.data(), NULL, useSnap ? snap.data() : NULL);
    }

    void ComponentGui<Transform<float>>::Show(entt::entity entity_id) {
        if (entity_id == entt::null || !Engine::State().all_of<Component<Transform<float>>>(entity_id)) {
            return;
        }

        auto &transform = Engine::State().get<Component<Transform<float>>>(entity_id);
        ComponentGui<Transform<float>>::Show(transform);
    }

    void ComponentGui<Transform<float>>::Edit(Transform<float> &transform){
        if (ImGui::Checkbox("Guizmo", &show_guizmo)) {
            if (show_guizmo) {
                ImGuizmo::Enable(true);
            } else {
                ImGuizmo::Enable(false);
            }
        }
        if (show_guizmo) {
            auto &camera = Engine::Context().get<Camera<float>>();
            EditTransform(camera, transform.model.matrix());
        }
    }

    void ComponentGui<Transform<float>>::Show(Component<Transform<float>> &component){
        auto &instances = Engine::Context().get<ResourceContainer<Transform<float>>>();
        return Show(instances.pool[component.index]);
    }

    void ComponentGui<Transform<float>>::Show(Transform<float> &transform) {
        Edit(transform);
        ImGui::Separator();

        if (ImGui::Button("Reset")) {
            transform.model.setIdentity();
        }


        ImGui::Text("position: (%f, %f, %f)", transform.get_position()[0], transform.get_position()[1],
                    transform.get_position()[2]);
        ImGui::Text("scale:    (%f, %f, %f)", transform.get_scale()[0], transform.get_scale()[1],
                    transform.get_scale()[2]);
        Eigen::Vector<float, 3> angle_axis = transform.get_angles_axis();
        ImGui::Text("rotation: (%f, %f, %f)", angle_axis[0], angle_axis[1], angle_axis[2]);

        ImGui::Separator();

        ImGui::Text("model matrix: %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n",
                    transform.model(0, 0), transform.model(0, 1), transform.model(0, 2), transform.model(0, 3),
                    transform.model(1, 0), transform.model(1, 1), transform.model(1, 2), transform.model(1, 3),
                    transform.model(2, 0), transform.model(2, 1), transform.model(2, 2), transform.model(2, 3),
                    transform.model(3, 0), transform.model(3, 1), transform.model(3, 2), transform.model(3, 3));

    }


    void ComponentGui<Transform<double>>::Show(entt::entity entity_id) {
        if (entity_id == entt::null || !Engine::State().all_of<Component<Transform<double>>>(entity_id)) {
            return;
        }

        auto &component = Engine::State().get<Component<Transform<double>>>(entity_id);
        ComponentGui<Transform<double>>::Show(component);
    }

    void ComponentGui<Transform<double>>::Edit(Transform<double> &transform){
        if (ImGui::Checkbox("Guizmo", &show_guizmo)) {
            if (show_guizmo) {
                ImGuizmo::Enable(true);
            } else {
                ImGuizmo::Enable(false);
            }
        }
        if (show_guizmo) {
            auto &camera = Engine::Context().get<Camera<float>>();
            auto model = transform.model.cast<float>();
            EditTransform(camera, model.matrix());
            transform.model = model.cast<double>();
        }
    }

    void ComponentGui<Transform<double>>::Show(Component<Transform<double>> &component) {
        auto &instances = Engine::Context().get<ResourceContainer<Transform<double>>>();
        return Show(instances.pool[component.index]);
    }

    void ComponentGui<Transform<double>>::Show(Transform<double> &transform) {
        Edit(transform);
        ImGui::Separator();

        if (ImGui::Button("Reset")) {
            transform.model.setIdentity();
        }


        ImGui::Text("position: (%f, %f, %f)", transform.get_position()[0], transform.get_position()[1],
                    transform.get_position()[2]);
        ImGui::Text("scale:    (%f, %f, %f)", transform.get_scale()[0], transform.get_scale()[1],
                    transform.get_scale()[2]);
        Eigen::Vector<double, 3> angle_axis = transform.get_angles_axis();
        ImGui::Text("rotation: (%f, %f, %f)", angle_axis[0], angle_axis[1], angle_axis[2]);

        ImGui::Separator();

        ImGui::Text("model matrix: %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n",
                    transform.model(0, 0), transform.model(0, 1), transform.model(0, 2), transform.model(0, 3),
                    transform.model(1, 0), transform.model(1, 1), transform.model(1, 2), transform.model(1, 3),
                    transform.model(2, 0), transform.model(2, 1), transform.model(2, 2), transform.model(2, 3),
                    transform.model(3, 0), transform.model(3, 1), transform.model(3, 2), transform.model(3, 3));

    }
}