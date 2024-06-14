//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_SYSTEMUNIFORM_H
#define ENGINE23_SYSTEMUNIFORM_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemUniform {
    public:
        SystemUniform() = default;

        ~SystemUniform() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMUNIFORM_H
