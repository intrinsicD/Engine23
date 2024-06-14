//
// Created by alex on 04.03.24.
//

#ifndef ENGINE23_PROPERTYGUI_H
#define ENGINE23_PROPERTYGUI_H

#include "ComponentGui.h"
#include "Property.h"

namespace Bcg{
    template<>
    struct ComponentGui<AbstractProperty*>{
        static void Show(AbstractProperty *aProperty);
    };

    template<typename T>
    struct ComponentGui<Property<T>>{
        static void Show(Property<T> &property);
    };

    template<>
    struct ComponentGui<PropertyContainer>{
        static void Show(PropertyContainer &property_container);
    };
}

#endif //ENGINE23_PROPERTYGUI_H
