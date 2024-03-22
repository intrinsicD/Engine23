//
// Created by alex on 05.02.24.
//

#ifndef ENGINE23_SYSTEMPOINTCLOUD_H
#define ENGINE23_SYSTEMPOINTCLOUD_H

#include "ComponentGui.h"

namespace Bcg{
    class SystemPointCloud {
    public:
        SystemPointCloud() = default;

        ~SystemPointCloud() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };

    template<>
    struct ComponentGui<PointCloud>{
        static void Show(entt::entity entity_id);
    };
}

#endif //ENGINE23_SYSTEMPOINTCLOUD_H
