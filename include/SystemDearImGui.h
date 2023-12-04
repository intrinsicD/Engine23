//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMDEARIMGUI_H
#define ENGINE23_SYSTEMDEARIMGUI_H

#include "System.h"

namespace Bcg {
    class SystemGui : protected System {
    public:
        SystemGui();

        ~SystemGui() override = default;

        void add_to_window(void *window);

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMDEARIMGUI_H
