//
// Created by alex on 03.05.24.
//

#ifndef ENGINE23_SYSTEMASSET_H
#define ENGINE23_SYSTEMASSET_H


#include "EngineFwd.h"

namespace Bcg {
    class SystemAsset {
    public:
        SystemAsset() = default;

        ~SystemAsset() = default;

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

#endif //ENGINE23_SYSTEMASSET_H
