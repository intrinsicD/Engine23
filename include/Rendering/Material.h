//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_MATERIAL_H
#define ENGINE23_MATERIAL_H

#include <string>

namespace Bcg{
    class Material{
    public:
        Material() = default;

        virtual ~Material() = default;

        virtual void set_shader_program() = 0;

        void set_shader_program(const std::string &shader_program_name);

        virtual void update_uniforms() = 0;

        unsigned int shader_program = 0;
    };
}

#endif //ENGINE23_MATERIAL_H
