//
// Created by alex on 21.05.24.
//

#include "SystemPicker.h"
#include "Engine.h"
#include "Commands.h"
#include "Events.h"
#include "Picker.h"
#include "imgui.h"
#include "Mouse.h"
#include "Window.h"
#include "Camera.h"
#include "Components.h"
#include "SystemRendererOpenGL.h"
#include "TypeName.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

//TODO set depth on click and compute the correct picker point positions...
namespace Bcg {
    namespace SystemPickerInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_mouse_button_press(const Events::Update<Mouse<float>::Button::Press> &event);
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

                bool is_background = picker.point.is_background;
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

        Point
        GetPickerPointFromWinCoords(float x, float y,
                                    int width, int height,
                                    const Eigen::Matrix<float, 4, 4> &view, const Eigen::Matrix<float, 4, 4> &proj,
                                    float hdpi = 1.0) {
            Point point;
            point.win_coords = {x * hdpi, y * hdpi};
            float depth = SystemRendererOpenGL::get_depth_at_screen_position(x, y);
            point.is_background = depth >= 1.0;
            point.norm_dev_coords = WindowCoordsToNormalizedDeviceCoordinates(point.win_coords, width, height);
            point.view = NormalizedDeviceCoordinatesToViewCoords(point.norm_dev_coords, depth, proj);
            point.world = ViewCoordsToWorldCoords(point.view, view);
            return point;
        }

        void on_update_mouse_button_press(const Events::Update<Mouse<float>::Button::Press> &event) {
            auto &mouse = Engine::Context().get<Mouse<float>>();
            auto &picker = Engine::Context().get<Picker>();
            picker.point = SystemPicker::get_picker_point_from_win_coords(mouse.position.current[0],
                                                                          mouse.position.current[1]);
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
        return TypeName<Picker>::name();
    }

    Point SystemPicker::get_picker_point_from_win_coords(float x, float y) {
        Components<Window> windows;
        auto &component_window = Engine::Context().get<Component<Window>>();
        auto &window = windows.get_instance(component_window);
        auto win_size = window.get_size();

        Components<Camera<float>> cameras;
        auto &component_camera = Engine::Context().get<Component<Camera<float>>>();
        auto &camera = cameras.get_instance(component_camera);
        return SystemPickerInternal::GetPickerPointFromWinCoords(x, y,
                                                                 win_size[0],
                                                                 win_size[1],
                                                                 camera.get_view(),
                                                                 camera.get_projection(),
                                                                 window.dpi);
    }

    void SystemPicker::pre_init() {
        Engine::Context().emplace<Picker>();
    }

    void SystemPicker::init() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemPickerInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Update<Mouse<float>::Button::Press>>().connect<&SystemPickerInternal::on_update_mouse_button_press>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemPicker::remove() {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemPickerInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Update<Mouse<float>::Button::Press>>().disconnect<&SystemPickerInternal::on_update_mouse_button_press>();
        Log::Info("Removed", name()).enqueue();
    }
}