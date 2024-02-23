//
// Created by alex on 22.02.24.
//

#ifndef ENGINE23_SYSTEMMESH_H
#define ENGINE23_SYSTEMMESH_H

#include "EngineFwd.h"
#include "components/ComponentGui.h"

namespace Bcg {
    class SystemMesh {
    public:
        SystemMesh() = default;

        ~SystemMesh() = default;

        static std::string name();

        static bool load(const std::string &filepath, entt::entity entity);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMMESH_H
