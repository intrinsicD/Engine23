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

        friend std::ostream &operator<<(std::ostream &stream, const Material &material) {
            stream << "Material: " << material.name << std::endl;
            stream << "Program index: " << material.program_idx << std::endl;
            stream << "Attributes: ";
            for (auto &attribute: material.attributes) {
                stream << attribute << " ";
            }
            stream << std::endl;
            stream << "Uniforms: ";
            for (auto &uniform: material.uniforms) {
                stream << uniform << " ";
            }
            stream << std::endl;
            stream << "Textures: ";
            for (auto &texture: material.textures) {
                stream << texture << " ";
            }
            stream << std::endl;
            return stream;
        }
    };
}

#endif //ENGINE23_MATERIAL_H
