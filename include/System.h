//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEM_H
#define ENGINE23_SYSTEM_H

#include "EngineFwd.h"
#include <string>
#include "entt/fwd.hpp"

namespace Bcg {
    class System {
    public:
        System() = default;

        explicit System(std::string name) : m_name(std::move(name)) {}

        virtual ~System() = default;

        const std::string &name() const {
            return m_name;
        }

        entt::entity id() const {
            return m_id;
        }

    protected:
        virtual void pre_init() = 0;

        virtual void init() = 0;

        virtual void remove() = 0;

        std::string m_name;
        entt::entity m_id;
    };
}

#endif //ENGINE23_SYSTEM_H
