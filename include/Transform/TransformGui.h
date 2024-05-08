//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_TRANSFORMGUI_H
#define ENGINE23_TRANSFORMGUI_H

#include "ComponentGui.h"
#include "Transform.h"
#include "Component.h"

namespace Bcg {
    template<>
    struct ComponentGui<Transform<float>> {
        static void Show(entt::entity entity_id);

        static void Show(Component<Transform<float>> &component);

        static void Show(Transform<float> &transform);

        static void Edit(Transform<float> &transform);
    };
}

#endif //ENGINE23_TRANSFORMGUI_H
