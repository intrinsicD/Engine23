//
// Created by alex on 08.05.24.
//

#ifndef ENGINE23_SYSTEMTEXTURES_H
#define ENGINE23_SYSTEMTEXTURES_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemTexture {
    public:
        SystemTexture() = default;

        ~SystemTexture() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMTEXTURES_H
