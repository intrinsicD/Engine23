//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_UNIFORM_H
#define ENGINE23_UNIFORM_H

#include <string>
#include <functional>

namespace Bcg {
    struct Uniform {
        std::string name;

        std::function<void(unsigned int program_id)> setter;
    };
}
#endif //ENGINE23_UNIFORM_H
