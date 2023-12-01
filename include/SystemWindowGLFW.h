//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMWINDOWGLFW_H
#define ENGINE23_SYSTEMWINDOWGLFW_H

#include "Events.h"
#include "EngineFwd.h"

namespace Bcg::System::Window::Glfw {
    void pre_init_system();

    void init_system();

    void remove_system();

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);

    void swap_and_poll_events();
}

#endif //ENGINE23_SYSTEMWINDOWGLFW_H
