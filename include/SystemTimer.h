//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMTIMER_H
#define ENGINE23_SYSTEMTIMER_H

#include "EngineFwd.h"
#include "Events.h"

namespace Bcg::System::Timer{
    void add_system();

    void remove_system();

    void begin_main_loop(Time &time);

    void end_main_loop(Time &time);

    void begin_simulation_loop(Time &time);

    void end_simulation_loop(Time &time);

    void on_startup(const Events::Startup<Engine> &event);

    void on_begin_frame(const Events::Begin<Frame> &event);

    void on_end_frame(const Events::End<Frame> &event);
}

#endif //ENGINE23_SYSTEMTIMER_H
