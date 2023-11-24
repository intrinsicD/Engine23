//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_FACTORIES_H
#define ENGINE23_FACTORIES_H

#include "Managers.h"
#include "Systems.h"

namespace Bcg {
    class Factory {
    public:
        Factory() = default;

        virtual ~Factory() = default;
    };

    class ManagerFactory : public Factory {
    public:
        ManagerFactory() = default;

        ~ManagerFactory() override = default;

        static CommandBufferManager *create_or_get_command_buffer_manager();

        static TimeManager *create_or_get_time_manager();

        static WorkerPoolManager *create_or_get_worker_pool_manager();

        static WindowManager *create_or_get_window_manager();
    };


    class SystemFactory : public Factory {
    public:
        SystemFactory() = default;

        ~SystemFactory() override = default;

        static CommandBufferSystem *create_or_get_command_buffer_system();

        static RenderSystem *create_or_get_render_system();

        static PhysicsSystem *create_or_get_physics_system();

        static AnimationSystem *create_or_get_animation_system();

        static AudioSystem *create_or_get_audio_system();

        static InputSystem *create_or_get_input_system();

        static ScriptingSystem *create_or_get_scripting_system();

        static NetworkSystem *create_or_get_network_system();

        static GuiSystem *create_or_get_gui_system();

        static PluginSystem *create_or_get_plugin_system();

        static WindowSystem *create_or_get_window_system();
    };
}

#endif //ENGINE23_FACTORIES_H
