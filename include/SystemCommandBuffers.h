//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMCOMMANDBUFFERS_H
#define ENGINE23_SYSTEMCOMMANDBUFFERS_H

#include "System.h"

namespace Bcg {
    class SystemCommandBuffers : protected System{
    public:
        SystemCommandBuffers();

        ~SystemCommandBuffers() override = default;

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMCOMMANDBUFFERS_H
