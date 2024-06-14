//
// Created by alex on 21.05.24.
//


#include "SystemKeyboard.h"
#include "Engine.h"
#include "imgui.h"
#include "Commands.h"
#include "Events.h"
#include "Keyboard.h"
#include "Picker.h"
#include "TypeName.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemKeyboardInternal {
        static bool show_gui = false;

        void on_startup(const Events::Startup<Engine> &events);

        void on_shutdown(const Events::Shutdown<Engine> &events);

        void RenderGuiKeyboard(const Keyboard &keyboard);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_keyboard(const Events::Update<Keyboard> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemKeyboardInternal {
        void on_startup(const Events::Startup<Engine> &events) {
            Engine::Dispatcher().sink<Events::Update<Keyboard>>().connect<&on_update_keyboard>();
            Log::Info("Startup", SystemKeyboard::name());
        }

        void on_shutdown(const Events::Shutdown<Engine> &events) {
            Engine::Dispatcher().sink<Events::Update<Keyboard>>().disconnect<&on_update_keyboard>();
            Log::Info("Shutdown", SystemKeyboard::name());
        }

        void RenderGuiKeyboard(const Keyboard &keyboard) {
            ImGui::Text("Key:         %d", keyboard.key);
            ImGui::Text("Scancode:    %d", keyboard.scancode);
            ImGui::Text("Action:      %d", keyboard.action);
            ImGui::Text("Mods:        %d", keyboard.mods);
            ImGui::Text("Num pressed: %d", keyboard.count_pressed);
            bool shift = keyboard.shift;
            bool alt = keyboard.alt;
            bool ctrl = keyboard.ctrl;
            bool esc = keyboard.esc;
            bool enter = keyboard.enter;
            bool gui_capture = keyboard.gui_capture;

            ImGui::Checkbox("Shift", &shift);
            ImGui::Checkbox("Alt", &alt);
            ImGui::Checkbox("Ctrl", &ctrl);
            ImGui::Checkbox("Esc", &esc);
            ImGui::Checkbox("Enter", &enter);
            ImGui::Checkbox("GuiCapture", &gui_capture);

            //now which

            std::vector<std::string> which_keys;
            for (const auto &key: keyboard.which) {
                which_keys.push_back(keyboard.get_key_string(key));
            }

            if (!which_keys.empty()) {
                static int selected = 0;
                ImGui::ListBox("Pressed Keys",
                               &selected,
                               [](void *data, int idx, const char **out_text) {
                                   auto &vector = *static_cast<std::vector<std::string> *>(data);
                                   *out_text = vector.at(idx).c_str();
                                   return true;
                               },
                               static_cast<void *>(&which_keys),
                               which_keys.size());
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Keyboard", &show_gui)) {
                RenderGuiKeyboard(Engine::Context().get<Keyboard>());
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu("Input")) {
                    if (ImGui::MenuItem("Keyboard", nullptr, &show_gui)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
        }


        void on_update_keyboard(const Events::Update<Keyboard> &event) {
            auto &keyboard = Engine::Context().get<Keyboard>();

            if (keyboard.which.contains(Keyboard::KEY::_DELETE)) {
                Engine::Dispatcher().trigger(Events::Destroy<entt::entity>{&Engine::Context().get<Picker>().id.entity});
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {

    std::string SystemKeyboard::name() {
        return "System" + component_name();
    }

    std::string SystemKeyboard::component_name() {
        return TypeName<Keyboard>::name();
    }

    void SystemKeyboard::pre_init() {
        auto &keyboard = Engine::Context().emplace<Keyboard>();
        keyboard.keys.resize(1024);
    }

    void SystemKeyboard::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemKeyboardInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemKeyboardInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemKeyboardInternal::on_update_gui_menu>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemKeyboard::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemKeyboardInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemKeyboardInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemKeyboardInternal::on_update_gui_menu>();
        Log::Info("Removed", name()).enqueue();
    }
}
