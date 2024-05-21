//
// Created by alex on 21.05.24.
//

#ifndef ENGINE23_SYSTEMPICKER_H
#define ENGINE23_SYSTEMPICKER_H


#include "EngineFwd.h"

namespace Bcg {
    class SystemPicker {
    public:
        SystemPicker() = default;

        ~SystemPicker() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMPICKER_H
