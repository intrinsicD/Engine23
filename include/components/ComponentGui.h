//
// Created by alex on 05.01.24.
//

#ifndef ENGINE23_COMPONENTGUI_H
#define ENGINE23_COMPONENTGUI_H

#include "EngineFwd.h"

namespace Bcg {
    template<typename T>
    struct ComponentGui {
        static void Show(entt::entity entity_id);
    };
}

#endif //ENGINE23_COMPONENTGUI_H
