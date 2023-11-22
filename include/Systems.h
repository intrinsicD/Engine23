//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_SYSTEMS_H
#define ENGINE23_SYSTEMS_H

#include <string>
#include "entt/fwd.hpp"

namespace Bcg{
    class System {
    public:
        //Is an Engine component, it lives inside the Engine's state (registry)
        //Adds core functionality to the Engine
        //Should be as general as possible
        //Should be as independent as possible from other components of the Engine
        //Has access to the Engine's state via Engine::get_instance()->state
        //Has access to the Engine's dispatcher via Engine::get_instance()->dispatcher
        //Exposes functions for performance critical stuff
        //has protected functions for commands and events

        explicit System(std::string name);

        virtual ~System() = default;

        virtual void update() {}

        std::string name;
        entt::entity id;
    };

    class PluginSystem : public System {
    public:
        PluginSystem();

        ~PluginSystem() override = default;
    };

    class WindowSystem : public System {
    public:
        WindowSystem();

        ~WindowSystem() override = default;
    };

    class RenderSystem : public System {
    public:
        RenderSystem();

        ~RenderSystem() override = default;
    };

    class PhysicsSystem : public System {
    public:
        PhysicsSystem();

        ~PhysicsSystem() override = default;
    };

    class AnimationSystem : public System {
    public:
        AnimationSystem();

        ~AnimationSystem() override = default;
    };

    class AudioSystem : public System {
    public:
        AudioSystem() ;

        ~AudioSystem() override = default;
    };

    class InputSystem : public System {
    public:
        InputSystem() ;

        ~InputSystem() override = default;
    };

    class ScriptingSystem : public System {
    public:
        ScriptingSystem() ;

        ~ScriptingSystem() override = default;
    };

    class NetworkSystem : public System {
    public:
        NetworkSystem() ;

        ~NetworkSystem() override = default;
    };

    class GuiSystem : public System {
    public:
        GuiSystem();

        ~GuiSystem() override = default;
    };
}

#endif //ENGINE23_SYSTEMS_H
