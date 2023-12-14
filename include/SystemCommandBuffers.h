//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMCOMMANDBUFFERS_H
#define ENGINE23_SYSTEMCOMMANDBUFFERS_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemCommandBuffers {
    public:
        SystemCommandBuffers() = default;

        ~SystemCommandBuffers() = default;

        static std::string name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMCOMMANDBUFFERS_H
