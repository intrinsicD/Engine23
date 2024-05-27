//
// Created by alex on 08.05.24.
//

#ifndef ENGINE23_TEXTURE_H
#define ENGINE23_TEXTURE_H

#include <ostream>

namespace Bcg {
    struct Texture {
        unsigned int id;
        int width;
        int height;
        int depth;
        unsigned int format;

        friend std::ostream &operator<<(std::ostream &stream, const Texture &texture) { return stream; }
    };
}

#endif //ENGINE23_TEXTURE_H
