//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_ENGINEFWD_H
#define ENGINE23_ENGINEFWD_H

#include "entt/fwd.hpp"

namespace Bcg{
    class Engine;
    struct Frame;
    struct CommandBuffer;
    struct Command;
    struct Time;
    struct MainLoop;
    struct SimulationLoop;
    struct Gui;
    struct GuiMenu;
    struct Plugin;
    struct Renderer;
    struct AABB;
    struct Hierarchy;
    class PointCloud;
    enum class LogLevel{
        Error = 0, Warn = 1, Info = 2
    };
}

#endif //ENGINE23_ENGINEFWD_H
