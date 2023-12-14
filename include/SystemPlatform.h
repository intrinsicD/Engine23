//
// Created by alex on 04.12.23.
//

#ifndef ENGINE23_SYSTEMPLATFORM_H
#define ENGINE23_SYSTEMPLATFORM_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemPlatform {
    public:
        SystemPlatform() = default;

        ~SystemPlatform() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMPLATFORM_H
