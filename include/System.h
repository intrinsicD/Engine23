//
// Created by alex on 11.06.24.
//

#ifndef ENGINE23_SYSTEM_H
#define ENGINE23_SYSTEM_H

#include "TypeStringification.h"
#include "EngineFwd.h"

namespace Bcg {
    template<typename T>
    class System {
    public:
        System() = default;

        ~System() = default;

        static std::string name() { return TypeName<T>::name; }

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEM_H
