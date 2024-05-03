//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMRENDEREROPENGL_H
#define ENGINE23_SYSTEMRENDEREROPENGL_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemRendererOpenGL {
    public:
        SystemRendererOpenGL() = default;

        ~SystemRendererOpenGL() = default;

        static std::string name();

        static void set_viewport(int x, int y, int width, int height);

        static void set_viewport(int width, int height);

        static void *create_window(int width, int height, const std::string &title);

        static void destroy(void *window_handle);

        static void make_current(void *window_handle);

        static void swap_buffers(void *window_handle);

        static void set_window_size(void *window_handle, int width, int height);

        static double get_dpi_for_monitor(void *monitor);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMRENDEREROPENGL_H
