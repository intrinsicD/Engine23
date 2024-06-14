//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_UNIFORMGUI_H
#define ENGINE23_UNIFORMGUI_H

#include "ComponentGui.h"
#include "Component.h"
#include "Uniform.h"

namespace Bcg {
    template<>
    struct ComponentGui<Uniform> {
        static void Show(Component<Uniform> &component);

        static void Show(Uniform &uniform);
    };
}

#endif //ENGINE23_UNIFORMGUI_H
