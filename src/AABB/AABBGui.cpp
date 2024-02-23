//
// Created by alex on 19.02.24.
//

#include "AABBGui.h"
#include "Engine.h"
#include "imgui.h"
#include <sstream>

namespace Bcg{
    void ComponentGui<AABB3>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<AABB3>(entity_id)) {
            ComponentGui<AABB3>::Show(Engine::State().get<AABB3>(entity_id));
        }
    }

    void ComponentGui<AABB3>::Show(AABB3 &aabb) {
        std::stringstream ss;
        ss << aabb.min;
        ImGui::Text("min: (%s)", ss.str().c_str());
        ss.clear(); //clear any error flags that might be set on the string stream
        ss.str(""); //remove the existing content from the string stream
        ss << aabb.max;
        ImGui::Text("max: (%s)", ss.str().c_str());
    }
}