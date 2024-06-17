//
// Created by alex on 17.06.24.
//
#include "UniformGui.h"
#include "imgui.h"

namespace Bcg {
    void ComponentGui<Uniform>::Show(Uniform &uniform) {
        ImGui::Text("Uniform: %s", uniform.name.c_str());
        ImGui::Text("ptr: %p", uniform.ptr);
        ImGui::Text("setter: %s", uniform.setter ? "is set" : "is not set");
    }
}