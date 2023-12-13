//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMPLUGINS_H
#define ENGINE23_SYSTEMPLUGINS_H

#include "System.h"

namespace Bcg {
    class SystemPlugins : protected System {
    public:
        SystemPlugins();

        ~SystemPlugins() override = default;

        static Plugin *load(const std::string &filepath);

        static void unload(void *handle);

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMPLUGINS_H
