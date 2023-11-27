//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMRENDERER_H
#define ENGINE23_SYSTEMRENDERER_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::Renderer{
    void add_system();

    void remove_system();

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);

    void on_begin_frame(const Events::Begin<Frame> &event);

    void on_end_frame(const Events::End<Frame> &event);
}

#endif //ENGINE23_SYSTEMRENDERER_H
