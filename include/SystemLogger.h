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

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMLOGGER_H
