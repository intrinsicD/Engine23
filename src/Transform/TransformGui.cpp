//
// Created by alex on 26.02.24.
//

#include "TransformGui.h"
#include "Transform.h"
#include "Engine.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Camera.h"
#include "glm/gtc/type_ptr.hpp"

namespace Bcg {
    static bool show_guizmo = false;

    inline void EditTransform(const Camera &camera, glm::mat4 &matrix) {
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
        glm::vec3 translation, rotation, scale;
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix),
                                              glm::value_ptr(translation),
                                              glm::value_ptr(rotation),
                                              glm::value_ptr(scale));
        ImGui::InputFloat3("Tr", glm::value_ptr(translation));
        ImGui::InputFloat3("Rt", glm::value_ptr(rotation));
        ImGui::InputFloat3("Sc", glm::value_ptr(scale));
        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(translation),
                                                glm::value_ptr(rotation),
                                                glm::value_ptr(scale),
                                                glm::value_ptr(matrix));

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
        static glm::vec3 snap = glm::vec3(1.0f);
        switch (mCurrentGizmoOperation) {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", &snap.x);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", &snap.x);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &snap.x);
                break;
            default:
                break;
        }
        ImGuiIO &io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::Manipulate(glm::value_ptr(camera.get_view()), glm::value_ptr(camera.get_projection()),
                             mCurrentGizmoOperation, mCurrentGizmoMode,
                             glm::value_ptr(matrix), NULL, useSnap ? &snap.x : NULL);
    }

    void ComponentGui<Transform>::Show(entt::entity entity_id) {
        if (entity_id == entt::null || !Engine::State().all_of<Transform>(entity_id)) {
            return;
        }

        auto &transform = Engine::State().get<Transform>(entity_id);
        ComponentGui<Transform>::Show(transform);
    }

    void ComponentGui<Transform>::Show(Bcg::Transform &transform) {
        if (ImGui::Checkbox("Guizmo", &show_guizmo)) {
            if (show_guizmo) {
                ImGuizmo::Enable(true);
            } else {
                ImGuizmo::Enable(false);
            }
        }
        if (show_guizmo) {
            auto &camera = Engine::Context().get<Camera>();
            EditTransform(camera, transform.model);
        }
        ImGui::Separator();

        if (ImGui::Button("Reset")) {
            transform.model = glm::mat4(1.0f);
        }

        ImGui::Text("position: (%f, %f, %f)", transform.get_position().x, transform.get_position().y,
                    transform.get_position().z);
        ImGui::Text("scale:    (%f, %f, %f)", transform.get_scale().x, transform.get_scale().y,
                    transform.get_scale().z);
        ImGui::Text("rotation: (%f, %f, %f)", transform.get_euler_angles().x, transform.get_euler_angles().y,
                    transform.get_euler_angles().z);

        ImGui::Separator();

        ImGui::Text("model matrix: %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n", transform.model[0][0], transform.model[0][1], transform.model[0][2],
                    transform.model[0][3],
                    transform.model[1][0], transform.model[1][1], transform.model[1][2], transform.model[1][3],
                    transform.model[2][0], transform.model[2][1], transform.model[2][2], transform.model[2][3],
                    transform.model[3][0], transform.model[3][1], transform.model[3][2], transform.model[3][3]);

    }
}