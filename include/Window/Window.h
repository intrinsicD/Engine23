//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_WINDOW_H
#define ENGINE23_WINDOW_H

#include <string>

namespace Bcg {
    struct Window {
        std::string title = "Viewer";
        int width = 800;
        int height = 600;
        double dpi = 1.0;
        float background_color[4] = {0.1f, 0.3f, 0.6f, 1.0f};

        template<typename T>
        T get_aspect() const {
            return static_cast<T>(width) / static_cast<T>(height);
        }
    };
}

#endif //ENGINE23_WINDOW_H
