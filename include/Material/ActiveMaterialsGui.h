//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_ACTIVEMATERIALSGUI_H
#define ENGINE23_ACTIVEMATERIALSGUI_H

#include "ComponentGui.h"
#include "Component.h"
#include "ActiveMaterials.h"

namespace Bcg {
    template<>
    struct ComponentGui<ActiveMaterials> {
        static void Show(entt::entity entity_id);

        static void Show(Component<ActiveMaterials> &component);

        static void Show(ActiveMaterials &active_materials);
    };
}
#endif //ENGINE23_ACTIVEMATERIALSGUI_H
