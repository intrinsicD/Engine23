//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMRENDERER_H
#define ENGINE23_SYSTEMRENDERER_H

#include "System.h"

namespace Bcg {
    class SystemRenderer : public System {
    public:
        SystemRenderer();

        ~SystemRenderer() override = default;

    protected:
        friend Engine;

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMRENDERER_H
