//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_INPUT_H
#define ENGINE23_INPUT_H

#include "Eigen/Core"
#include <vector>
#include <string>

namespace Bcg {
    struct Input {
        struct Mouse {
        struct Position : public Eigen::Vector<float, 2> {
            };
            struct Button {
                bool left = false;
                bool right = false;
                bool middle = false;
            };
            struct Scroll : public Eigen::Vector<float, 2> {
            };
            Position position;
            Position position_delta;
            Position last_left_click;
            Position last_middle_click;
            Position last_right_click;
            Position last_drag_pos;
            Scroll scroll;
            Button button;
            bool gui_capture = false;
        } mouse;
        struct Keyboard {
            std::vector<bool> keys;
            bool gui_capture = false;
        } keyboard;
        struct Drop {
            std::vector<std::string> paths;
        } drop;
    };
}

#endif //ENGINE23_INPUT_H
