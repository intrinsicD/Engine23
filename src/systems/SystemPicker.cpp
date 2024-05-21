//
// Created by alex on 21.05.24.
//

#include "SystemPicker.h"
#include "Engine.h"
#include "Commands.h"
#include "Events.h"
#include "Picker.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

//TODO set depth on click and compute the correct picker point positions...
namespace Bcg {
    namespace SystemPickerInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemPickerInternal {
        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Picker", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();

                bool is_background = picker.is_background;
                ImGui::Checkbox("Is Background", &is_background);
                ImGui::InputFloat("Point Selection Radius", &picker.point_selection_radius);
                if (ImGui::TreeNode("ID")) {
                    ImGui::InputScalar("Entity", ImGuiDataType_U32, &picker.id.entity);
                    ImGui::InputScalar("Vertex", ImGuiDataType_U32, &picker.id.vertex);
                    ImGui::InputScalar("Edge", ImGuiDataType_U32, &picker.id.edge);
                    ImGui::InputScalar("Face", ImGuiDataType_U32, &picker.id.face);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Point")) {
                    ImGui::InputFloat3("Object", picker.point.object.data());
                    ImGui::InputFloat3("World", picker.point.world.data());
                    ImGui::InputFloat3("View", picker.point.view.data());
                    ImGui::TreePop();
                }
                const char *listbox_items[] = {"Entities", "Points", "Vertices", "Edges", "Faces"};
                static int listbox_item_current = static_cast<int>(picker.mode);
                ImGui::ListBox("Selection Mode", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 5);
                picker.mode = static_cast<Picker::SelectionMode>(listbox_item_current);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Picker", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemPicker::name() {
        return "System" + component_name();
    }

    std::string SystemPicker::component_name() {
        return "Picker";
    }

    void SystemPicker::pre_init() {
        Engine::Context().emplace<Picker>();
    }

    void SystemPicker::init() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemPickerInternal::on_update_gui_menu>();
        Log::Info("Initialized", name());
    }

    void SystemPicker::remove() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemPickerInternal::on_update_gui_menu>();
        Log::Info("Removed", name());
    }
}