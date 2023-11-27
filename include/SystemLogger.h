//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMLOGGER_H
#define ENGINE23_SYSTEMLOGGER_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::Logger{
    void add_system();

    void remove_system();

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);

    void set_log_level(LogLevel level);
}

#endif //ENGINE23_SYSTEMLOGGER_H
