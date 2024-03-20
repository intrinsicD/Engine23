//
// Created by alex on 20.03.24.
//

#ifndef ENGINE23_RENDERPASS_H
#define ENGINE23_RENDERPASS_H

#include "entt/entity/fwd.hpp"
#include <set>

namespace Bcg {
    class RenderPass {
    public:
        using Container_t = std::set<entt::entity>;
        using Iterator_t = Container_t::iterator;
        using ConstIterator_t = Container_t::const_iterator;

        RenderPass() = default;

        virtual ~RenderPass() = default;

        bool add(entt::entity entity_id);

        bool remove(entt::entity entity_id);

        Iterator_t begin();

        Iterator_t end();

        ConstIterator_t begin() const;

        ConstIterator_t end() const;

        virtual void execute() const = 0;

    protected:
        unsigned int m_program_id;
        Container_t m_entities;
    };
}

#endif //ENGINE23_RENDERPASS_H
