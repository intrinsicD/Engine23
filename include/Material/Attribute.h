//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_ATTRIBUTE_H
#define ENGINE23_ATTRIBUTE_H

#include <string>

namespace Bcg {
    struct Attribute {
        std::string shader_attribute_name;
        unsigned int location;
        unsigned int buffer_idx;
        unsigned int dims;
        bool normalized;

        friend std::ostream &operator<<(std::ostream &stream, const Attribute &attribute) {
            stream << "Shader Attribute Name: " << attribute.shader_attribute_name << "\n";
            stream << "Location: " << attribute.location << "\n";
            stream << "Buffer Idx: " << attribute.buffer_idx << "\n";
            stream << "Dims: " << attribute.dims << "\n";
            stream << "Normalized: " << attribute.normalized << "\n";
            return stream;
        }
    };
}

#endif //ENGINE23_ATTRIBUTE_H
