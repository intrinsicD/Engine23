//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMUSERINPUT_H
#define ENGINE23_SYSTEMUSERINPUT_H

#include "EngineFwd.h"

namespace Bcg{
    class SystemUserInput {
    public:
        SystemUserInput() = default;

        ~SystemUserInput() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMUSERINPUT_H
