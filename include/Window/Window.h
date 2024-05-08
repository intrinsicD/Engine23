//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_WINDOW_H
#define ENGINE23_WINDOW_H

#include <string>
#include "Eigen/Core"

namespace Bcg {
    struct Window {
        std::string title = "Viewer";
        int create_width = 800;
        int create_height = 600;
        Eigen::Vector<float, 4> background_color = {0.1f, 0.3f, 0.6f, 1.0f};

        double dpi = 1.0;
        void *window_handle = nullptr;

        [[nodiscard]] Eigen::Vector<int, 2> get_size() const;

        template<typename T>
        T get_aspect() const {
            Eigen::Vector<int, 2> size = std::move(get_size());
            return static_cast<T>(size[0]) / static_cast<T>(size[1]);
        }

        friend std::ostream &operator<<(std::ostream &stream, const Window &window) {
            stream << "Title: " << window.title << "\n size: " << window.get_size().transpose() << "\n dpi: "
                   << window.dpi << "\n";
            return stream;
        }
    };
}

#endif //ENGINE23_WINDOW_H
