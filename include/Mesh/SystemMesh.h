//
// Created by alex on 22.02.24.
//

#ifndef ENGINE23_SYSTEMMESH_H
#define ENGINE23_SYSTEMMESH_H

#include "EngineFwd.h"
#include "ComponentGui.h"
#include "Component.h"

namespace Bcg {
    class SystemMesh {
    public:
        SystemMesh() = default;

        ~SystemMesh() = default;

        static std::string name();

        static std::string component_name();

        static bool load(const std::string &filepath, entt::entity entity);

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
#endif //ENGINE23_SYSTEMMESH_H
