//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_SYSTEMAABB_H
#define ENGINE23_SYSTEMAABB_H

#include "EngineFwd.h"
#include "ComponentGui.h"

namespace Bcg {
    class SystemAABB {
    public:
        SystemAABB() = default;

        ~SystemAABB() = default;

        static std::string name();

        static std::string component_name();

        static unsigned int create_instance();

        static void destroy_instance(unsigned int instance_id);

        static void add_to_entity(entt::entity entity_id, unsigned int instance_id);

        static void remove_from_entity(entt::entity entity_id);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMAABB_H
