//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMUSERINPUT_H
#define ENGINE23_SYSTEMUSERINPUT_H

#include "System.h"

namespace Bcg{
    class SystemUserInput : public System {
    public:
        SystemUserInput();

        ~SystemUserInput() override = default;

    protected:
        friend Engine;

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMUSERINPUT_H
