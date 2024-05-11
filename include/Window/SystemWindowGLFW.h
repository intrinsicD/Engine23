//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMWINDOWGLFW_H
#define ENGINE23_SYSTEMWINDOWGLFW_H

#include "EngineFwd.h"

namespace Bcg{
    class SystemWindowGLFW {
    public:
        SystemWindowGLFW() = default;

        ~SystemWindowGLFW() = default;

        static std::string name();

        static std::string component_name();

        static void set_window_close(void *window_handle);

        static void set_window_resize(void *window_handle, int width, int height);

        static void swap_and_poll_events();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMWINDOWGLFW_H
