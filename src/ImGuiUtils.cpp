//
// Created by alex on 10.01.24.
//

#include "ImGuiUtils.h"
#include "GlmUtils.h"

namespace Bcg {
    namespace ImGuiUtilsInternal {
        static auto StringVectorGetter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<std::string> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = vector.at(idx).c_str();
            return true;
        };
        static auto GlmVec2Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::vec2> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
        static auto GlmVec3Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::vec3> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
        static auto GlmVec4Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::vec4> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
        static auto GlmUVec2Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::uvec2> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
        static auto GlmUVec3Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::uvec3> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
        static auto GlmUVec4Getter = [](void *vec, int idx, const char **out_text) {
            auto &vector = *static_cast<std::vector<glm::uvec4> *>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = ToString(vector.at(idx)).c_str();
            return true;
        };
    }
    namespace ImGuiUtils {
        void ShowHelpMarker(const char *desc) {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(desc);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        void ShowHelpMarker(const std::string &desc) {
            ShowHelpMarker(desc.c_str());
        }

        template<typename T>
        static bool ComboBox(const char *label, int *current_item_index, const std::vector<T> &items) {
            auto current_item = std::to_string(*current_item_index) + ": " + ToString(items.at(*current_item_index));
            bool value_changed = false;
            if (ImGui::BeginCombo(label, current_item.c_str())) {
                for (int n = 0; n < items.size(); n++) {
                    bool is_selected = (*current_item_index == n);
                    if (ImGui::Selectable((std::to_string(n)+ ": " + ToString(items[n])).c_str(), is_selected)) {
                        *current_item_index = n;
                        value_changed = true;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            return value_changed;
        }

        bool Combo(const char *label, int *current_item, const std::vector<std::string> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec2> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec3> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec4> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec2> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec3> &items) {
            return ComboBox(label, current_item, items);
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec4> &items) {
            return ComboBox(label, current_item, items);
        }
    }
}