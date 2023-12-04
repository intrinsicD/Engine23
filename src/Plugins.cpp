//
// Created by alex on 22.11.23.
//

#include "Plugins.h"
#include "entt/entity/entity.hpp"

namespace Bcg{
    Plugin::Plugin(std::string name) : System(std::move(name)) {}
}