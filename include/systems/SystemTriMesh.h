//
// Created by alex on 10.01.24.
//

#ifndef ENGINE23_SYSTEMTRIMESH_H
#define ENGINE23_SYSTEMTRIMESH_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemTriMesh {
    public:
        SystemTriMesh() = default;

        ~SystemTriMesh() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}


#endif //ENGINE23_SYSTEMTRIMESH_H
