//
// Created by alex on 05.02.24.
//

#include "components/Picker.h"
#include "Engine.h"

namespace Bcg{
    bool Picker::ID::check_entity() const {
        return entity != entt::null && Engine::State().valid(entity);
    }
}