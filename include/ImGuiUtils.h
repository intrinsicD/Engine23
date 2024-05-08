//
// Created by alex on 10.01.24.
//

#ifndef ENGINE23_IMGUIUTILS_H
#define ENGINE23_IMGUIUTILS_H

#include <string>
#include <sstream>
#include <vector>
#include "imgui.h"
#include "glm/glm.hpp"
#include "Components.h"

namespace Bcg {
    namespace ImGuiUtils {
        void ShowHelpMarker(const char *desc);

        void ShowHelpMarker(const std::string &desc);

        // Function to convert various types to string

        bool Combo(const char *label, int *current_item, const std::vector<std::string> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec2> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec3> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec4> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec2> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec3> &items);

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec4> &items);

        template<typename T>
        void Show(Components<T> &components) {
            for (size_t instance_id = 0; instance_id < components.get_size(); ++instance_id) {
                if (ImGui::TreeNode(std::to_string(instance_id).c_str())) {
                    ComponentGui<T>::Show(components.get_instance(instance_id));
                    ImGui::TreePop();
                }
            }
        }
    }
}

#endif //ENGINE23_IMGUIUTILS_H
