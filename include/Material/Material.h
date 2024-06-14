//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_MATERIAL_H
#define ENGINE23_MATERIAL_H

#include <vector>
#include <string>

namespace Bcg {
    struct Material {
        std::string name;
        unsigned int program_idx;
        std::vector<unsigned int> attributes;
        std::vector<unsigned int> uniforms;
        std::vector<unsigned int> textures;
    };
}

#endif //ENGINE23_MATERIAL_H
