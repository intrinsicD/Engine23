//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMUSERINPUT_H
#define ENGINE23_SYSTEMUSERINPUT_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::SystemUserInput{
    void pre_init_system();

    void init_system();

    void remove_system();

    void on_begin_frame(const Events::Begin<Frame> &event);
}

#endif //ENGINE23_SYSTEMUSERINPUT_H
