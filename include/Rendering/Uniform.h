//
// Created by alex on 23.04.24.
//

#ifndef ENGINE23_UNIFORM_H
#define ENGINE23_UNIFORM_H

#include <string>

namespace Bcg {
    struct Uniform {
        std::string name;
        void *ptr = nullptr;
        unsigned int size = 0;
        unsigned int type = 0;
        unsigned int id;
    };
}

#endif //ENGINE23_UNIFORM_H
