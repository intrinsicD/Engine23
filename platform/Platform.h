//
// Created by alex on 23.11.23.
//

#ifndef ENGINE23_PLATFORM_H
#define ENGINE23_PLATFORM_H

#include <string>

namespace Bcg{
    class Platform{
    public:
        Platform();

        virtual ~Platform() = default;

        std::string os;
        std::string arch;
        std::string compiler;
    };
}

#endif //ENGINE23_PLATFORM_H
