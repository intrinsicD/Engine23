//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMTIMER_H
#define ENGINE23_SYSTEMTIMER_H

#include "EngineFwd.h"

namespace Bcg{
    class SystemTimer {
    public:
        SystemTimer() = default;

        ~SystemTimer() = default;

        static std::string name();
    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMTIMER_H
