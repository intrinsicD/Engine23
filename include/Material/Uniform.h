//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_UNIFORM_H
#define ENGINE23_UNIFORM_H

#include <string>
#include <functional>

namespace Bcg {
    struct Uniform {
        std::string name;

        void *ptr;
        std::function<void(unsigned int program_id)> setter;

        friend std::ostream &operator<<(std::ostream &stream, const Uniform &uniform) {
            stream << "Name: " << uniform.name << "\n";
            stream << "ptr: " << uniform.ptr << "\n";
            stream << "setter: " << (uniform.setter ? "is set" : "is not set") << "\n";
            return stream;
        }
    };
}
#endif //ENGINE23_UNIFORM_H
