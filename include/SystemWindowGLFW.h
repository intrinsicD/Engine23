//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMWINDOWGLFW_H
#define ENGINE23_SYSTEMWINDOWGLFW_H

#include "System.h"

namespace Bcg{
    class SystemWindowGLFW : public System {
    public:
        SystemWindowGLFW();

        ~SystemWindowGLFW() override = default;

        static void swap_and_poll_events();

    protected:
        friend Engine;
        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMWINDOWGLFW_H
