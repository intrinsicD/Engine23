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

        System(std::string name) : m_name(std::move(name)) {}

        virtual ~System() = default;

        bool is_initialized() const {
            return m_is_initialized;
        }

        const std::string &name() const {
            return m_name;
        }

        entt::entity id() const {
            return m_id;
        }

    protected:
        virtual void pre_init_system() = 0;

        virtual void init_system() = 0;

        virtual void remove_system() = 0;

        bool m_is_initialized = false;
        std::string m_name;
        entt::entity m_id;
    };
}

#endif //ENGINE23_SYSTEM_H
