//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_TRANSFORMGUI_H
#define ENGINE23_TRANSFORMGUI_H

#include "components/ComponentGui.h"

namespace Bcg{
    struct Transform;

    template<>
    struct ComponentGui<Transform> {
        static void Show(entt::entity entity_id);
    };
}

#endif //ENGINE23_TRANSFORMGUI_H
