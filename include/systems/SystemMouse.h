//
// Created by alex on 26.04.24.
//

#ifndef ENGINE23_SYSTEMMOUSE_H
#define ENGINE23_SYSTEMMOUSE_H

#include "EngineFwd.h"

namespace Bcg{
    class SystemMouse {
    public:
        SystemMouse() = default;

        ~SystemMouse() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMMOUSE_H
