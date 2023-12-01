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

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMCOMMANDBUFFERS_H
