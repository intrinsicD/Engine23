//
// Created by alex on 27.11.23.
//

#include "systems/SystemUserInput.h"
#include "Events/Events.h"
#include "Engine.h"
#include "Commands.h"
#include "imgui.h"
#include "Input.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemUserInputInternal {
        void RenderGuiMouse(const Input::Mouse &mouse);

        void RenderGuiKeyboard(const Input::Keyboard &keyboard);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_mouse_button(const Events::Update<Input::Mouse::Button> &event);

        void on_update_input_drop(const Events::Update<Input::Drop> &event);

        void on_startup(const Events::Startup<Engine> &events);

        void on_shutdown(const Events::Shutdown<Engine> &events);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemUserInputInternal {
        void RenderGuiMouse(const Input::Mouse &mouse) {
            ImGui::Text("Position: (%f, %f)", mouse.position[0], mouse.position[1]);
            ImGui::Text("Scroll: (%f, %f)", mouse.scroll[0], mouse.scroll[1]);
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

        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event){
            if(!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

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
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event){
            if (ImGui::BeginMenu("Menu")) {
                if(ImGui::MenuItem("Input", nullptr, &show_gui)){
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_mouse_button(const Events::Update<Input::Mouse::Button> &event){
            auto &input = Engine::Context().get<Input>();
            if(input.mouse.button.left){
                input.mouse.last_left_click = input.mouse.position;
            }
            if(input.mouse.button.middle){
                input.mouse.last_middle_click = input.mouse.position;
            }
            if(input.mouse.button.right){
                input.mouse.last_right_click = input.mouse.position;
            }
        }

        void on_update_input_drop(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            for(auto &path : input.drop.paths){
                Log::Info("Dropped: " + path).enqueue();
            }
        }

        void on_startup(const Events::Startup<Engine> &events) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemUserInputInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Update<Input::Drop>>().connect<&SystemUserInputInternal::on_update_input_drop>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Button>>().connect<&SystemUserInputInternal::on_update_mouse_button>();
            Log::Info(SystemUserInput::name() , "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &events) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemUserInputInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Update<Input::Drop>>().connect<&SystemUserInputInternal::on_update_input_drop>();
            Engine::Dispatcher().sink<Events::Update<Input::Mouse::Button>>().connect<&SystemUserInputInternal::on_update_mouse_button>();
            Log::Info(SystemUserInput::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemUserInput::name() {
        return "SystemUserInput";
    }

    void SystemUserInput::pre_init() {
        auto &input = Engine::Context().emplace<Input>();
        input.keyboard.keys.resize(1024);
    }

    void SystemUserInput::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemUserInputInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemUserInputInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemUserInput::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemUserInputInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemUserInputInternal::on_shutdown>();
        Log::Info("Removed", name()).enqueue();
    }
}