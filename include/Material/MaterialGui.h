//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_MATERIALGUI_H
#define ENGINE23_MATERIALGUI_H

#include "ComponentGui.h"
#include "Material.h"

namespace Bcg {
    template<>
    struct ComponentGui<Material> {
        static void Show(unsigned int material_id);

        static void Show(Material &material);
    };
}

#endif //ENGINE23_MATERIALGUI_H
