//
// Created by alex on 26.04.24.
//

#ifndef ENGINE23_INTERNALCALLBACKEVENTS_H
#define ENGINE23_INTERNALCALLBACKEVENTS_H

namespace Bcg::Events::Callback {
    struct WindowSize {
        void *window_ptr;
        int width;
        int height;
    };

    struct MouseButton {
        void *window_ptr;
        int button;
        int action;
        int mods;
    };

    struct MouseCursorPosition {
        void *window_ptr;
        float x, y;
    };

    struct MouseScroll {
        void *window_ptr;
        float x, y;
    };

    struct Key {
        void *window_ptr;
        int key;
        int scancode;
        int action;
        int mods;
    };

    struct Drop {
        void *window_ptr;
        int count;
        const char **paths;
    };
}


#endif //ENGINE23_INTERNALCALLBACKEVENTS_H
