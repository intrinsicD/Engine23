//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_ATTRIBUTE_H
#define ENGINE23_ATTRIBUTE_H

#include <string>

namespace Bcg{
    struct Attribute{
        std::string shader_attribute_name;
        unsigned int location;
        unsigned int buffer_idx;
        unsigned int dims;
        bool normalized;
    };
}

#endif //ENGINE23_ATTRIBUTE_H
