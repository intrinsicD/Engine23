//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMDEARIMGUI_H
#define ENGINE23_SYSTEMDEARIMGUI_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemGui {
    public:
        SystemGui() = default;

        ~SystemGui() = default;

        static std::string name();

        static void add_to_window(void *window);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMDEARIMGUI_H
