//
// Created by alex on 04.12.23.
//

#ifndef ENGINE23_SYSTEMPLATFORM_H
#define ENGINE23_SYSTEMPLATFORM_H

#include "System.h"

namespace Bcg {
    class SystemPlatform : public System {
    public:
        SystemPlatform();

        ~SystemPlatform() override = default;

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMPLATFORM_H
