//
// Created by alex on 05.01.24.
//

#include "SystemTransform.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "components/Transform.h"
#include "components/Picker.h"
#include "components/Camera.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {
        static bool show_gui = false;
        static bool show_guizmo = false;
        static bool enable_guizmo = false;

        void EditTransform(const Camera &camera, glm::mat4 &matrix);

        void on_render_gui(const Events::Render<Gui> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {
        void EditTransform(const Camera &camera, glm::mat4 &matrix) {
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

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemTransformInternal::on_render_gui>();
                return;
            }

            if (ImGui::Begin("Transform", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                if (picker.id.entity != entt::null) {
                    auto &transform = Engine::State().get<Transform>(picker.id.entity);
                    if (ImGui::Button("Reset")) {
                        SystemTransform::set_identity(picker.id.entity);
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox("Guizmo", &show_guizmo);
                    if (show_guizmo) {
                        if (ImGui::Checkbox("Enable", &enable_guizmo)) {
                            if (enable_guizmo) {
                                ImGuizmo::Enable(true);
                            } else {
                                ImGuizmo::Enable(false);
                            }
                        }
                        if (enable_guizmo) {
                            auto &camera = Engine::Context().get<Camera>();
                            EditTransform(camera, transform.model);
                        }
                    }

                    ComponentGui<Transform>::Show(picker.id.entity);
                }
            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Transform", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemTransformInternal::on_render_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemTransformInternal::on_render_gui_menu>();
            Log::Info(SystemTransform::name() + ": Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemTransformInternal::on_render_gui_menu>();
            Log::Info(SystemTransform::name() + ": Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTransform::name() {
        return "SystemTransform";
    }

    void SystemTransform::set_identity(entt::entity entity) {
        auto &transform = Engine::State().get<Transform>(entity);
        transform.model = glm::mat4(1.0f);
    }

    void SystemTransform::pre_init() {

    }

    void SystemTransform::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemTransformInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemTransform::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemTransformInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info(name() + ": Removed").enqueue();
    }
}