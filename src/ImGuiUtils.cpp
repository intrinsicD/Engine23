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

        bool Combo(const char *label, int *current_item, const std::vector<std::string> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::StringVectorGetter,
                                const_cast<std::vector<std::string> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec2> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmVec2Getter,
                                const_cast<std::vector<glm::vec2> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec3> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmVec3Getter,
                                const_cast<std::vector<glm::vec3> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::vec4> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmVec4Getter,
                                const_cast<std::vector<glm::vec4> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec2> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmUVec2Getter,
                                const_cast<std::vector<glm::uvec2> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec3> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmUVec3Getter,
                                const_cast<std::vector<glm::uvec3> *>(&items), items.size());
        }

        bool Combo(const char *label, int *current_item, const std::vector<glm::uvec4> &items) {
            return ImGui::Combo(label, current_item, ImGuiUtilsInternal::GlmUVec4Getter,
                                const_cast<std::vector<glm::uvec4> *>(&items), items.size());
        }


    }
}