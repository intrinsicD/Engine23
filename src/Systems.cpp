//
// Created by alex on 22.11.23.
//

#include "Systems.h"
#include "entt/entity/entity.hpp"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------

    System::System(std::string name) : name(std::move(name)), id(entt::null) {}

    //------------------------------------------------------------------------------------------------------------------

    PluginSystem::PluginSystem() : System("Plugins") {}

    //------------------------------------------------------------------------------------------------------------------

    WindowSystem::WindowSystem() : System("Window") {}

    //------------------------------------------------------------------------------------------------------------------

    RenderSystem::RenderSystem() : System("Render") {}

    //------------------------------------------------------------------------------------------------------------------

    PhysicsSystem::PhysicsSystem() : System("Physics") {}

    //------------------------------------------------------------------------------------------------------------------

    AnimationSystem::AnimationSystem() : System("Animation") {}

    //------------------------------------------------------------------------------------------------------------------

    AudioSystem::AudioSystem() : System("Audio") {}

    //------------------------------------------------------------------------------------------------------------------

    InputSystem::InputSystem() : System("Input") {}

    //------------------------------------------------------------------------------------------------------------------

    ScriptingSystem::ScriptingSystem() : System("Scripting") {}

    //------------------------------------------------------------------------------------------------------------------

    NetworkSystem::NetworkSystem() : System("Network") {}

    //------------------------------------------------------------------------------------------------------------------

    GuiSystem::GuiSystem() : System("Gui") {}

    //------------------------------------------------------------------------------------------------------------------
}