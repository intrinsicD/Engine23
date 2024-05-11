//
// Created by alex on 08.05.24.
//

#include "Window.h"
#include "GLFW/glfw3.h"

namespace Bcg{
    Eigen::Vector<int, 2> Window::get_size() const{
        if(!window_handle){
            return {width, height};
        }

        Eigen::Vector<int, 2> size;
        glfwGetWindowSize(static_cast<GLFWwindow *>(window_handle), &size[0], &size[1]);
        return size;
    }
}