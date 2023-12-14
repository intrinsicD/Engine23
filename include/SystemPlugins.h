//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMPLUGINS_H
#define ENGINE23_SYSTEMPLUGINS_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemPlugins {
    public:
        SystemPlugins() = default;

        ~SystemPlugins() = default;

        static std::string name();

        static Plugin *load(const std::string &filepath);

        static void unload(void *handle);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMPLUGINS_H
