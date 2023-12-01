//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMLOGGER_H
#define ENGINE23_SYSTEMLOGGER_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::Logger{
    void pre_init_system();

    void init_system();

    void remove_system();

    void set_log_level(LogLevel level);
}

#endif //ENGINE23_SYSTEMLOGGER_H
