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
    }
}

#endif //ENGINE23_IMGUIUTILS_H
