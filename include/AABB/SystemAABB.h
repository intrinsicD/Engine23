//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_SYSTEMAABB_H
#define ENGINE23_SYSTEMAABB_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemAABB {
    public:
        SystemAABB() = default;

        ~SystemAABB() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMAABB_H
