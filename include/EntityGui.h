//
// Created by alex on 10.06.24.
//

#ifndef ENGINE23_ENTITYGUI_H
#define ENGINE23_ENTITYGUI_H

#include "Entity.h"
#include "ComponentGui.h"

namespace Bcg{
    template<>
    struct ComponentGui<Entity> {
        static void Show(entt::entity entity_id);
    };
}
#endif //ENGINE23_ENTITYGUI_H
