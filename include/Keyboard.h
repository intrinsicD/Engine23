//
// Created by alex on 26.04.24.
//

#ifndef ENGINE23_KEYBOARD_H
#define ENGINE23_KEYBOARD_H

#include <vector>

namespace Bcg {
    class Keyboard {
    public:
        Keyboard() = default;

        enum KEY{
            _A = 0,
            _B,
            _C,
            _D,
            _E,
            _F,
            _G,
            _H,
            _I,
            _J,
            _K,
            _L,
            _M,
            _N,
            _O,
            _P,
            _Q,
            _R,
            _S,
            _T,
            _U,
            _V,
            _W,
            _X,
            _Y,
            _Z,
            _0,
            _1,
            _2,
            _3,
            _4,
            _5,
            _6,
            _7,
            _8,
            _9,
            _F1,
            _F2,
            _F3,
            _F4,
            _F5,
            _F6,
            _F7,
            _F8,
            _F9,
            _F10,
            _F11,
            _F12,
            _UP,
            _DOWN,
            _LEFT,
            _RIGHT,
            _SPACE,
            _BACKSPACE,
            _DELETE,
            _ENTER,
            _ESCAPE,
            _ALT,
            _STRG,
            _SHIFT,
            _TAB,
            _BACKTAB,
            last_counter
        };

        struct Key {
            Key() : which(KEY::last_counter){

            }

            unsigned int count_pressed;
            unsigned int last_action;
            unsigned int last_pressed;
            unsigned int last_released;

            bool any() const { return count_pressed > 0; }

            std::vector<bool> which;
        } key;

        std::vector<bool> keys;
        bool gui_capture = false;
    };
}

#endif //ENGINE23_KEYBOARD_H
