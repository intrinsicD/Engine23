//
// Created by alex on 19.01.24.
//

#ifndef ENGINE23_SYSTEMHIERARCHY_H
#define ENGINE23_SYSTEMHIERARCHY_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemHierarchy {
    public:
        SystemHierarchy() = default;

        ~SystemHierarchy() = default;

        static std::string name();

        static bool has_component(entt::entity &entity_id);

        static Hierarchy &get_or_add(entt::entity entity_id);

        static bool destroy(entt::entity entity_id);

        static bool detach_from_parent(entt::entity entity_id);

        static void attach_child(entt::entity parent_id, entt::entity child_id);

        static std::vector<entt::entity> get_children(entt::entity entity_id);

        static entt::entity get_root(entt::entity entity_id);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMHIERARCHY_H
