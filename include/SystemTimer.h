//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMTIMER_H
#define ENGINE23_SYSTEMTIMER_H

#include "EngineFwd.h"
#include "Events.h"

namespace Bcg::System::Timer{
    void pre_init_system();

    void init_system();

    void remove_system();

    void begin_main_loop(Time &time);

    void end_main_loop(Time &time);

    void begin_simulation_loop(Time &time);

    void end_simulation_loop(Time &time);

    void render_gui(Time &time);

    void on_startup(const Events::Startup<Engine> &event);

    void on_begin_frame(const Events::Begin<Frame> &event);
}

#endif //ENGINE23_SYSTEMTIMER_H
