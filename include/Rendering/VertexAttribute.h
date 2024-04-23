//
// Created by alex on 23.04.24.
//

#ifndef ENGINE23_VERTEXATTRIBUTE_H
#define ENGINE23_VERTEXATTRIBUTE_H

#include <string>

namespace Bcg{
    struct VertexAttribute{
        std::string name;
        unsigned int type;
        unsigned int size;
        unsigned int offset;
    };
}

#endif //ENGINE23_VERTEXATTRIBUTE_H
