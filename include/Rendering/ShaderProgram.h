//
// Created by alex on 21.03.24.
//

#ifndef ENGINE23_SHADERPROGRAM_H
#define ENGINE23_SHADERPROGRAM_H

#include <vector>
#include "VertexAttribute.h"

namespace Bcg {
    class ShaderProgram {
    public:
        ShaderProgram() = default;

        unsigned int id;

        virtual std::vector<VertexAttribute> get_required_vertex_attributes() = 0;

    };
}

#endif //ENGINE23_SHADERPROGRAM_H
