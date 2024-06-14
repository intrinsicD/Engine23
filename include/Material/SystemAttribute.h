//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_SYSTEMATTRIBUTE_H
#define ENGINE23_SYSTEMATTRIBUTE_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemAttribute {
    public:
        SystemAttribute() = default;

        ~SystemAttribute() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}
#endif //ENGINE23_SYSTEMATTRIBUTE_H
