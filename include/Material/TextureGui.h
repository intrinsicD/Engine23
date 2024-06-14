//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_TEXTUREGUI_H
#define ENGINE23_TEXTUREGUI_H

#include "ComponentGui.h"
#include "Component.h"
#include "Texture.h"

namespace Bcg {
    template<>
    struct ComponentGui<Texture> {
        static void Show(entt::entity entity_id);

        static void Show(Component<Texture> &component);

        static void Show(Texture &texture);
    };
}

#endif //ENGINE23_TEXTUREGUI_H
