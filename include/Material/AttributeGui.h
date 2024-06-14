//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_ATTRIBUTEGUI_H
#define ENGINE23_ATTRIBUTEGUI_H

#include "ComponentGui.h"
#include "Component.h"
#include "Attribute.h"

namespace Bcg {
    template<>
    struct ComponentGui<Attribute> {
        static void Show(Attribute &attribute);
    };
}

#endif //ENGINE23_ATTRIBUTEGUI_H
