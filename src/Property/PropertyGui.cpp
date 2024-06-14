
//
// Created by alex on 13.06.24.
//

#include "PropertyGui.h"
#include "TypeName.h"
#include "imgui.h"
#include "ImGuiUtils.h"

namespace Bcg {
    void ComponentGui<AbstractProperty*>::Show(AbstractProperty *aProperty) {
        if (!aProperty) return;
        ImGui::Text("Dims: %zu", aProperty->get_dims());
        ImGui::Text("Size: %zu", aProperty->get_size());
        ImGui::Separator();
        static int current_idx = 0;
        if(aProperty->as_strings().size() != aProperty->get_size()){
            aProperty->update_string_cache();
        }
        const auto &strings = aProperty->as_strings();
        bool selected_new_idx = ImGuiUtils::Combo(aProperty->get_name().c_str(), &current_idx, strings);
    }

    template<typename T>
    void ComponentGui<Property<T>>::Show(Property<T> &property) {
        if (!property) return;

        ImGui::Text("TypeName: %s", TypeName<T>::name());
        Show(property.get_const_sptr());
    }

    void ComponentGui<PropertyContainer>::Show(PropertyContainer &container) {
        if(ImGui::CollapsingHeader(container.get_name().c_str())){
            for(const auto &item : container.storage()){
                ComponentGui<AbstractProperty*>::Show(item.second.get());
            }
        }
    }

}