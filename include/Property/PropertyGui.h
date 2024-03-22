//
// Created by alex on 04.03.24.
//

#ifndef ENGINE23_PROPERTYGUI_H
#define ENGINE23_PROPERTYGUI_H

#include "ComponentGui.h"
#include "Property.h"
#include "imgui.h"


namespace Bcg{
    template<typename T>
    struct ComponentGui<Property<T>>{
        void Show(Property<T> &property){
            ImGui::Text(property.get_name().c_str());
            if(!property) return;
            ImGui::Text("Dims: %zu", property.get_dims());
            ImGui::Text("Size: %zu", property.get_size());
            ImGui::Separator();

        }
    };
}

#endif //ENGINE23_PROPERTYGUI_H
