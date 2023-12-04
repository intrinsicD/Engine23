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

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMTIMER_H
