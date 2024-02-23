//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_AABBGUI_H
#define ENGINE23_AABBGUI_H

#include "components/ComponentGui.h"
#include "EngineFwd.h"
#include "AABB.h"

namespace Bcg {
    template<>
    struct ComponentGui<AABB3> {
        static void Show(entt::entity entity_id);
        static void Show(AABB3 &aabb);
    };
}

#endif //ENGINE23_AABBGUI_H
