//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_SYSTEMMATERIAL_H
#define ENGINE23_SYSTEMMATERIAL_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemMaterial {
    public:
        SystemMaterial() = default;

        ~SystemMaterial() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMMATERIAL_H
