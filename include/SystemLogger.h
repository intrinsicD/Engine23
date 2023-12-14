//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMLOGGER_H
#define ENGINE23_SYSTEMLOGGER_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemLogger {
    public:
        SystemLogger() = default;

        ~SystemLogger() = default;

        static std::string name();

        static void set_log_level(LogLevel level);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMLOGGER_H
