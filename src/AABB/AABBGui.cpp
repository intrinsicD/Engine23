//
// Created by alex on 19.02.24.
//

#include "AABBGui.h"
#include "Engine.h"
#include "imgui.h"
#include <sstream>
#include "Components.h"
#include "SystemAABB.h"

namespace Bcg {
    void ComponentGui<AABB3>::Show(entt::entity entity_id) {
        if (entity_id != entt::null && Engine::State().all_of<Component<AABB3>>(entity_id)) {
            ComponentGui<AABB3>::Show(Engine::State().get<Component<AABB3>>(entity_id));
        }
    }

    void ComponentGui<AABB3>::Show(Component<AABB3> &component) {
        Components<AABB3> aabbs;
        return Show(aabbs.get_instance(component));
    }

    void ComponentGui<AABB3>::Show(AABB3 &aabb) {
        std::stringstream ss;
        ss << aabb.min.transpose();
        ImGui::Text("min: (%s)", ss.str().c_str());
        ss.clear(); //clear any error flags that might be set on the string stream
        ss.str(""); //remove the existing content from the string stream
        ss << aabb.max.transpose();
        ImGui::Text("max: (%s)", ss.str().c_str());
    }
}