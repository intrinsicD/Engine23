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

        enum KEY {
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
            _CTRL,
            _SHIFT,
            _TAB,
            last_counter
        };

        int key;
        int scancode;
        int action;
        int mods;
        int count_pressed;
        bool shift = false;
        bool alt = false;
        bool ctrl = false;
        bool esc = false;
        bool enter = false;

        std::vector<bool> keys;
        std::set<KEY> which;
        bool gui_capture = false;

        void set_key(KEY key, bool is_pressed) {
            keys[key] = is_pressed;
            if (is_pressed) {
                which.emplace(key);
            } else {
                which.erase(key);
            }
        }

        std::string get_key_string(KEY key) const {
            //Map each KEY to a string readable

            switch (key) {
                case _A:
                    return "A";
                case _B:
                    return "B";
                case _C:
                    return "C";
                case _D:
                    return "D";
                case _E:
                    return "E";
                case _F:
                    return "F";
                case _G:
                    return "G";
                case _H:
                    return "H";
                case _I:
                    return "I";
                case _J:
                    return "J";
                case _K:
                    return "K";
                case _L:
                    return "L";
                case _M:
                    return "M";
                case _N:
                    return "N";
                case _O:
                    return "O";
                case _P:
                    return "P";
                case _Q:
                    return "Q";
                case _R:
                    return "R";
                case _S:
                    return "S";
                case _T:
                    return "T";
                case _U:
                    return "U";
                case _V:
                    return "V";
                case _W:
                    return "W";
                case _X:
                    return "X";
                case _Y:
                    return "Y";
                case _Z:
                    return "Z";
                case _0:
                    return "0";
                case _1:
                    return "1";
                case _2:
                    return "2";
                case _3:
                    return "3";
                case _4:
                    return "4";
                case _5:
                    return "5";
                case _6:
                    return "6";
                case _7:
                    return "7";
                case _8:
                    return "8";
                case _9:
                    return "9";
                case _F1:
                    return "F1";
                case _F2:
                    return "F2";
                case _F3:
                    return "F3";
                case _F4:
                    return "F4";
                case _F5:
                    return "F5";
                case _F6:
                    return "F6";
                case _F7:
                    return "F7";
                case _F8:
                    return "F8";
                case _F9:
                    return "F9";
                case _F10:
                    return "F10";
                case _F11:
                    return "F11";
                case _F12:
                    return "F12";
                case _UP:
                    return "UP";
                case _DOWN:
                    return "DOWN";
                case _LEFT:
                    return "LEFT";
                case _RIGHT:
                    return "RIGHT";
                case _SPACE:
                    return "SPACE";
                case _BACKSPACE:
                    return "BACKSPACE";
                case _DELETE:
                    return "DELETE";
                case _ENTER:
                    return "ENTER";
                case _ESCAPE:
                    return "ESCAPE";
                case _ALT:
                    return "ALT";
                case _CTRL:
                    return "CTRL";
                case _SHIFT:
                    return "SHIFT";
                case _TAB:
                    return "TAB";
                default:
                    return "";
            }
        }
    };
}

#endif //ENGINE23_KEYBOARD_H
