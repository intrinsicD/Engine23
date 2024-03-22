//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_TRANSFORMGUI_H
#define ENGINE23_TRANSFORMGUI_H

#include "ComponentGui.h"

namespace Bcg {
    struct Transform;

    template<>
    struct ComponentGui<Transform> {
        static void Show(entt::entity entity_id);

        static void Show(Transform &transform);
    };
}

#endif //ENGINE23_TRANSFORMGUI_H
