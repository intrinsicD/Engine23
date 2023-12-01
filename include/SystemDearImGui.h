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

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMDEARIMGUI_H
