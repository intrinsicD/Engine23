//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMTIMER_H
#define ENGINE23_SYSTEMTIMER_H

#include "System.h"

namespace Bcg{
    class SystemTimer : public System {
    public:
        SystemTimer();

        ~SystemTimer() override = default;

    protected:
        friend Engine;

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMTIMER_H
