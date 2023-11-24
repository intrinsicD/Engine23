//
// Created by alex on 22.11.23.
//

#include "Systems.h"
#include "Components.h"
#include "Engine.h"

#include <iostream>

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------

    System::System(std::string name) : name(std::move(name)), id(entt::null) {}

    //------------------------------------------------------------------------------------------------------------------

    CommandBufferSystem::CommandBufferSystem() : System("Command Buffer") {
        auto *engine = Engine::Instance();
        if (engine && !engine->state.ctx().find<CommandBufferSystem>()) {
            auto &dispatcher = engine->dispatcher;
            dispatcher.sink<Events::Startup<Engine>>().connect<&CommandBufferSystem::on_startup>();
            dispatcher.sink<Events::Shutdown<Engine>>().connect<&CommandBufferSystem::on_shutdown>();
            dispatcher.sink<Events::Update<Engine>>().connect<&CommandBufferSystem::on_update>();

            auto &state = engine->state;
            auto &self = state.ctx().emplace<CommandBufferSystem>();
            self.id = state.create();
            state.emplace<std::vector<CommandBuffer *>>(self.id);
            auto cmd_buffer_id = state.create();
            state.emplace<CommandBuffer>(cmd_buffer_id);
            state.emplace<std::string>(cmd_buffer_id, "Main Command Buffer");
            state.emplace<Priority>(cmd_buffer_id, 0);
        }
    }

    bool CommandBufferSystem::add_command_buffer(std::string name, int priority) {
        auto &state = Engine::Instance()->state;
        auto &self = state.ctx().get<CommandBufferSystem>();
        if (state.valid(self.id)) {
            auto &buffers = state.get<std::vector<CommandBuffer *>>(self.id);

            auto view = state.view<CommandBuffer, std::string, Priority>();
            Priority last = {-1};
            for (auto entity: view) {
                auto &buffer = view.get<CommandBuffer>(entity);
                auto &buffer_name = view.get<std::string>(entity);
                auto &buffer_priority = view.get<Priority>(entity);
                if (last.value < buffer_priority.value) {
                    last = buffer_priority;
                } else {
                    std::cout << "CommandBufferSystem::add_command_buffer: Command buffer priorities are not sorted"
                              << std::endl;
                }
                if (buffer_name == name) {
                    return false;
                }
            }

            auto cmd_buffer_id = state.create();
            state.emplace<CommandBuffer>(cmd_buffer_id);
            state.emplace<std::string>(cmd_buffer_id, name);
            state.emplace<Priority>(cmd_buffer_id, priority);

            state.sort<Priority>([&](const auto &lhs, const auto &rhs) {
                return lhs.value < rhs.value;
            });
        }
    }

    bool CommandBufferSystem::remove_command_buffer(std::string name) {

    }

    int CommandBufferSystem::get_command_buffer_priority(std::string name) {

    }

    std::vector<CommandBuffer *> CommandBufferSystem::get_command_buffers(int priority) {

    }

    void CommandBufferSystem::execute_current_buffers() {

    }

    void CommandBufferSystem::swap_command_buffers_priority(int priority_a, int priority_b) {

    }

    void CommandBufferSystem::clear_and_prepare_next_buffers() {

    }

    void CommandBufferSystem::on_startup(const Events::Startup<Engine> &event) {

    }

    void CommandBufferSystem::on_shutdown(const Events::Shutdown<Engine> &event) {

    }

    void CommandBufferSystem::on_update(const Events::Update<Engine> &event) {

    }

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