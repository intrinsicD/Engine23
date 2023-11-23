//
// Created by alex on 23.11.23.
//

#ifndef ENGINE23_WINDOWMANAGERGLFW_H
#define ENGINE23_WINDOWMANAGERGLFW_H

#include "Managers.h"

namespace Bcg{
    class WindowManagerGLFW : public WindowManager{
    public:
        void create_window(int width, int height, std::string title) override;

        void destroy_window() override;

        void resize_window(int width, int height) const override;

        void set_window_title(std::string title) const override;

        double get_aspect_ratio() const override;

        void begin_frame() const override;

        void end_frame() const override;

    protected:
        WindowManagerGLFW();

        friend ManagerFactory;
    };
}

#endif //ENGINE23_WINDOWMANAGERGLFW_H
