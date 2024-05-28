//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_INPUT_H
#define ENGINE23_INPUT_H

#include <vector>
#include <string>

namespace Bcg {
    struct Input {
        struct Drop {
            std::vector<std::string> paths;
        } drop;
    };
}

#endif //ENGINE23_INPUT_H
