//
// Created by alex on 08.05.24.
//

#ifndef ENGINE23_CAMERAGUI_H
#define ENGINE23_CAMERAGUI_H

#include "Camera.h"
#include "EngineFwd.h"
#include "ComponentGui.h"
#include "Component.h"

namespace Bcg{
    template<>
    struct ComponentGui<Camera<float>> {
        static void Show(entt::entity entity_id);

        static void Show(Component<Camera<float>> &component);

        static void Show(Camera<float> &camera);
    };
}



#endif //ENGINE23_CAMERAGUI_H
