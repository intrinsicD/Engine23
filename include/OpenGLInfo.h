//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_OPENGLINFO_H
#define ENGINE23_OPENGLINFO_H

#include <string>

namespace Bcg {
    struct OpenglInfo {
        int major_hint = 4;
        int minor_hint = 6;
        int major;
        int minor;
        bool core = true;
        bool forward_compatibility = true;
        bool debug = true;
        std::string vendor;
        std::string renderer;
        std::string version;
        std::string glsl_version;
    };
}

#endif //ENGINE23_OPENGLINFO_H
