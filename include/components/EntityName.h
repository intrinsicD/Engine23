//
// Created by alex on 05.01.24.
//

#ifndef ENGINE23_ENTITYNAME_H
#define ENGINE23_ENTITYNAME_H

#include "EngineFwd.h"
#include "ComponentGui.h"

namespace Bcg {
    struct EntityName : public std::string {
        using std::string::string;
    };

    template<>
    struct ComponentGui<EntityName> {
        static void Show(entt::entity entity_id);
    };
}

#endif //ENGINE23_ENTITYNAME_H
