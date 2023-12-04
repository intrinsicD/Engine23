//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_PLUGINS_H
#define ENGINE23_PLUGINS_H

#include <string>
#include "entt/fwd.hpp"
#include "System.h"

namespace Bcg {
    class Plugin : public System {
    public:
        //Is an Engine component, it lives inside the Engine's state (registry)
        //Adds exchangeable functionality, e.g. formats or third party code to the Engine
        //Should be specialized
        //Should be as independent as possible from other components of the Engine
        //Has access to the Engine's state via Engine::get_instance()->state
        //Has access to the Engine's dispatcher via Engine::get_instance()->dispatcher
        //Exposes functions for performance critical stuff
        //has protected functions for commands and events

        explicit Plugin(std::string name);

        ~Plugin() override = default;
    };
}

#endif //ENGINE23_PLUGINS_H
