//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMLOGGER_H
#define ENGINE23_SYSTEMLOGGER_H

#include "System.h"

namespace Bcg {
    class SystemLogger : protected System {
    public:
        SystemLogger();

        ~SystemLogger() override = default;

        void set_log_level(LogLevel level);
    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMLOGGER_H
