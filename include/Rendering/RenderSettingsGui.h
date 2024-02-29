//
// Created by alex on 29.02.24.
//

#ifndef ENGINE23_RENDERSETTINGSGUI_H
#define ENGINE23_RENDERSETTINGSGUI_H

#include "components/ComponentGui.h"

namespace Bcg {
    class RenderSettings;
    template<>
    struct ComponentGui<RenderSettings> {
        void Show(entt::entity entity_id);

        void Show(RenderSettings &settings);
    };
}

#endif //ENGINE23_RENDERSETTINGSGUI_H
