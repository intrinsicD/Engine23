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

        Plugin *load(const std::string &filepath);

        void unload(void *handle);

    protected:
        friend Engine;

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMPLUGINS_H
