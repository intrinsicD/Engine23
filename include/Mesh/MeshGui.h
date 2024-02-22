//
// Created by alex on 22.02.24.
//

#ifndef ENGINE23_MESHGUI_H
#define ENGINE23_MESHGUI_H

#include "Mesh.h"
#include "EngineFwd.h"
#include "components/ComponentGui.h"

namespace Bcg{

    template<>
    struct ComponentGui<Mesh> {
        static void Show(entt::entity entity_id);
        static void Show(Mesh &mesh);
    };

}

#endif //ENGINE23_MESHGUI_H
