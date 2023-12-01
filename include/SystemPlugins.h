//
// Created by alex on 01.12.23.
//

#ifndef ENGINE23_SYSTEMPLUGINS_H
#define ENGINE23_SYSTEMPLUGINS_H

#include "EngineFwd.h"
#include "Events.h"
#include <string>

namespace Bcg::System::Plugins{
    void pre_init_system();

    void init_system();

    void remove_system();

    void on_startup(Events::Startup<Engine> &event);

    void on_shutdown(Events::Shutdown<Engine> &event);

    Plugin *load(const std::string &filepath);

    void unload(void *handle);
}

#endif //ENGINE23_SYSTEMPLUGINS_H
