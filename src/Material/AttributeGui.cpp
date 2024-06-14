//
// Created by alex on 14.06.24.
//

#include "AttributeGui.h"
#include "imgui.h"

namespace Bcg {
    void ComponentGui<Attribute>::Show(Bcg::Attribute &attribute) {
        ImGui::Text("Shader Attribute Name: %s", attribute.shader_attribute_name.c_str());
        ImGui::Text("Location: %u", attribute.location);
        ImGui::Text("Buffer Idx: %u", attribute.buffer_idx);
        ImGui::Text("Dims: %u", attribute.dims);
        ImGui::Checkbox("Normalized", &attribute.normalized);
    }
}