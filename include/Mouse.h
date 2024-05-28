//
// Created by alex on 26.04.24.
//

#ifndef ENGINE23_MOUSE_H
#define ENGINE23_MOUSE_H

#include "Eigen/Core"
#include "ClickPoint.h"

namespace Bcg {
    template<typename T>
    class Mouse {
    public:
        Mouse() = default;

        enum State {
            IDLE = 0,
            MOVE,
            DRAG,
            SCROLL
        } state;

        struct Button {
            struct Press{};
            struct Release{};

            int button = -1;
            int action = -1;
            int mods = -1;

            bool left = false;
            bool right = false;
            bool middle = false;

            bool any() const { return left || middle || right; }
        } button;

        struct Position {
            Eigen::Vector<T, 2> current;
            Eigen::Vector<T, 2> delta;
            Eigen::Vector<T, 2> last_left_click;
            Eigen::Vector<T, 2> last_middle_click;
            Eigen::Vector<T, 2> last_right_click;
            Eigen::Vector<T, 2> last_drag_pos;
        } position;

        using Scroll = Eigen::Vector<T, 2>;
        Scroll scroll;
        bool gui_capture = false;
    };
}

#endif //ENGINE23_MOUSE_H
