//
// Created by alex on 05.01.24.
//

#ifndef ENGINE23_SYSTEMTRANSFORM_H
#define ENGINE23_SYSTEMTRANSFORM_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemTransform {
    public:
        SystemTransform() = default;

        ~SystemTransform() = default;

        static std::string name();

        static std::string component_name();

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMTRANSFORM_H
