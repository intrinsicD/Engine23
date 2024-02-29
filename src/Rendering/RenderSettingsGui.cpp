//
// Created by alex on 29.02.24.
//

#include "RenderSettingsGui.h"
#include "RenderSettings.h"
#include "imgui.h"
#include "Engine.h"

namespace Bcg {
    void ComponentGui<RenderSettings>::Show(entt::entity entity_id) {
        if (entity_id == entt::null ||
            !Engine::State().valid(entity_id) ||
            !Engine::State().all_of<RenderSettings>(entity_id)
                    ) {
            auto &settings = Engine::State().get<RenderSettings>(entity_id);
            ComponentGui<RenderSettings>::Show(settings);
        }
    }

    void ComponentGui<RenderSettings>::Show(RenderSettings &settings) {
        //TODO maybe start here by defining what i want to have ... for rendering options
    }
}