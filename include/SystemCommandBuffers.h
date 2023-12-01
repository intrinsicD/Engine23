//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMCOMMANDBUFFERS_H
#define ENGINE23_SYSTEMCOMMANDBUFFERS_H

#include "EngineFwd.h"
#include "Events.h"

namespace Bcg::System::CommandBuffers {
    void pre_init_system();

    void init_system();

    void remove_system();

    void on_startup(Events::Startup<Engine> &event);

    void on_shutdown(Events::Shutdown<Engine> &event);
}

#endif //ENGINE23_SYSTEMCOMMANDBUFFERS_H
