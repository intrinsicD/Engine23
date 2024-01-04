//
// Created by alex on 04.01.24.
//

#ifndef ENGINE23_SYSTEMENTITY_H
#define ENGINE23_SYSTEMENTITY_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemEntity {
    public:
        SystemEntity() = default;

        ~SystemEntity() = default;

        static std::string name();

        static entt::entity create_entity();

        static void destroy_entity(entt::entity entity);

        static void set_name(entt::entity entity, const std::string &name);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMENTITY_H
