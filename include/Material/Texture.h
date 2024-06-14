//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_TEXTURE_H
#define ENGINE23_TEXTURE_H

#include <string>

namespace Bcg {
    struct Texture {
        std::string name;
        unsigned int id;
        unsigned int width;
        unsigned int height;
        unsigned int depth;

        friend std::ostream &operator<<(std::ostream &stream, const Texture &transform) {
            stream << "Name: " << transform.name << std::endl;
            stream << "ID: " << transform.id << std::endl;
            stream << "Width: " << transform.width << std::endl;
            stream << "Height: " << transform.height << std::endl;
            stream << "Depth: " << transform.depth << std::endl;
            return stream;
        }
    };
}

#endif //ENGINE23_TEXTURE_H
