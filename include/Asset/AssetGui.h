//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_ASSETGUI_H
#define ENGINE23_ASSETGUI_H

#include "ComponentGui.h"
#include "Component.h"

namespace Bcg {
    struct Asset;

    template<>
    struct ComponentGui<Asset> {
        static void Show(entt::entity entity_id);

        static void Show(Component<Asset> component);

        static void Show(Asset &asset);
    };
}
#endif //ENGINE23_ASSETGUI_H
