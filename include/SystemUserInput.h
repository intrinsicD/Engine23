//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMUSERINPUT_H
#define ENGINE23_SYSTEMUSERINPUT_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::UserInput{
    void add_system();

    void remove_system();

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);
}

#endif //ENGINE23_SYSTEMUSERINPUT_H
