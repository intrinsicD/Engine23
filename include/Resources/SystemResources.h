//
// Created by alex on 02.05.24.
//

#ifndef ENGINE23_SYSTEMRESOURCES_H
#define ENGINE23_SYSTEMRESOURCES_H

#include "EngineFwd.h"
#include "ResourceContainer.h"

namespace Bcg {
    class SystemResources {
    public:
        SystemResources() = default;

        ~SystemResources() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMRESOURCES_H
