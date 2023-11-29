//
// Created by alex on 27.11.23.
//

#include "SystemUserInput.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"
#include "imgui.h"

namespace Bcg::System::UserInput {
    void add_system() {
        if (!Engine::Context().find<Input>()) {
            Engine::Context().emplace<Input>();
            Log::Info("SystemUserInput: Added Input Component to Context").enqueue();
        }
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
        Log::Info("SystemUserInput: Added").enqueue();
    }

    void remove_system() {
        if (Engine::Context().find<Input>()) {
            Engine::Context().erase<Input>();
            Log::Info("SystemUserInput: Removed Input Component from Context").enqueue();
        }
        Log::Info("SystemUserInput: Removed").enqueue();
    }

    static bool show_gui = false;

    void RenderGuiMouse(const Input::Mouse &mouse) {
        ImGui::Text("Position: (%f, %f)", mouse.position.x, mouse.position.y);
        ImGui::Text("Scroll: (%f, %f)", mouse.scroll.x, mouse.scroll.y);
        ImGui::Text("Buttons: (%d, %d, %d)", mouse.button.left, mouse.button.middle,
                    mouse.button.right);
    }

    void RenderGuiKeyboard(const Input::Keyboard &keyboard) {
        ImGui::Text("Keys: ");
        for (int i = 0; i < keyboard.keys.size(); ++i) {
            if (keyboard.keys[i]) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%d", i);
            }
        }
    }

    void on_begin_frame(const Events::Begin<Frame> &event) {
        auto &double_buffer = Engine::State().ctx().get<CommandDoubleBuffer>();
        double_buffer.current->emplace_back(std::make_shared<TaskCommand>("GuiMenu", [&]() {
            if (ImGui::BeginMenu("Menu")) {
                ImGui::MenuItem("Input", nullptr, &show_gui);
                ImGui::EndMenu();
            }
            return 1;
        }));

        if (show_gui) {
            double_buffer.current->emplace_back(std::make_shared<TaskCommand>("GuiWindow", [&]() {
                if (ImGui::Begin("Input", &show_gui)) {
                    auto &input = Engine::Context().get<Input>();
                    if (ImGui::CollapsingHeader("Mouse")) {
                        ImGui::Indent();
                        RenderGuiMouse(input.mouse);
                        ImGui::Unindent();
                    }

                    if (ImGui::CollapsingHeader("Keyboard")) {
                        ImGui::Indent();
                        RenderGuiKeyboard(input.keyboard);
                        ImGui::Unindent();
                    }
                }
                ImGui::End();
                return 1;
            }));
        }
    }
}