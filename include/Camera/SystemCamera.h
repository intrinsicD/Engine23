//
// Created by alex on 15.12.23.
//

#ifndef ENGINE23_SYSTEMCAMERA_H
#define ENGINE23_SYSTEMCAMERA_H

#include "EngineFwd.h"

namespace Bcg{
    class SystemCamera {
    public:
        SystemCamera() = default;

        ~SystemCamera() = default;

        static std::string name();

        static std::string component_name();

        static void make_arc_ball_camera();
    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMCAMERA_H
