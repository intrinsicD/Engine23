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

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMRENDEREROPENGL_H
