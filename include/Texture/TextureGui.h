//
// Created by alex on 27.05.24.
//

#ifndef ENGINE23_TEXTUREGUI_H
#define ENGINE23_TEXTUREGUI_H

#include "ComponentGui.h"
#include "Texture.h"
#include "Component.h"

namespace Bcg {
    template<>
    struct ComponentGui<Texture> {
        static void Show(entt::entity entity_id);

        static void Show(Component<Texture> &component);

        static void Show(Texture &transform);
    };
}

#endif //ENGINE23_TEXTUREGUI_H
