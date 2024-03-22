//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_HIERARCHYGUI_H
#define ENGINE23_HIERARCHYGUI_H

#include "ComponentGui.h"

namespace Bcg{
    struct Hierarchy;

    template<>
    struct ComponentGui<Hierarchy> {
        static void Show(entt::entity entity_id);
        
        static void Show(Hierarchy &hierarchy);
    };
}

#endif //ENGINE23_HIERARCHYGUI_H
