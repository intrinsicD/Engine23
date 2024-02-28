//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_SYSTEMSUTILS_H
#define ENGINE23_SYSTEMSUTILS_H

#include "fmt/core.h"
#include "Commands.h"

namespace Bcg{
    template<class System>
    void on_construct_component(entt::registry &registry, entt::entity entity_id){
        Log::Info(fmt::format("Entity {} constructed component {}",
                              static_cast<unsigned int>(entity_id), System::component_name())).enqueue();
    }

    template<class System>
    void on_update_component(entt::registry &registry, entt::entity entity_id){
        Log::Info(fmt::format("Entity {} updated component {}",
                              static_cast<unsigned int>(entity_id), System::component_name())).enqueue();

    }

    template<class System>
    void on_destroy_component(entt::registry &registry, entt::entity entity_id){
        Log::Info(fmt::format("Entity {} destroyed component {}",
                              static_cast<unsigned int>(entity_id), System::component_name())).enqueue();
    }
}

#endif //ENGINE23_SYSTEMSUTILS_H
