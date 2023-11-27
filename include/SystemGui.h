//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMGUI_H
#define ENGINE23_SYSTEMGUI_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::Gui {
    void add_system();

    void remove_system();

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);

    void on_begin_frame(const Events::Begin<Frame> &event);

    void on_end_frame(const Events::End<Frame> &event);

    void add_to_window(void *window);
}

#endif //ENGINE23_SYSTEMGUI_H
