//
// Created by alex on 01.12.23.
//

#include "SystemCommandBuffers.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"

namespace Bcg::System::CommandBuffers{
    void on_update_simulation_command_double_buffer(const Events::Update<SimulationCommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<SimulationCommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    void on_update_render_command_double_buffer(const Events::Update<RenderCommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<RenderCommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    void on_update_command_double_buffer(const Events::Update<CommandDoubleBuffer> &event) {
        auto &double_buffer = Engine::Context().get<CommandDoubleBuffer>();
        for (auto &command: *double_buffer.p_current) {
            command->execute();
        }

        double_buffer.clear_and_swap();
    }

    void pre_init_system(){
        auto &state = Engine::State();
        auto *command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<CommandDoubleBuffer>(command_buffer_current, command_buffer_next);

        auto *simulation_command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *simulation_command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<SimulationCommandDoubleBuffer>(simulation_command_buffer_current, simulation_command_buffer_next);

        auto *render_command_buffer_current = &state.emplace<CommandBuffer>(state.create());
        auto *render_command_buffer_next = &state.emplace<CommandBuffer>(state.create());
        Engine::Context().emplace<RenderCommandDoubleBuffer>(render_command_buffer_current, render_command_buffer_next);
    }

    void init_system(){
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&on_shutdown>();
        Engine::Instance()->dispatcher.sink<Events::Update<SimulationCommandDoubleBuffer>>().connect<&on_update_simulation_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<RenderCommandDoubleBuffer>>().connect<&on_update_render_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<CommandDoubleBuffer>>().connect<&on_update_command_double_buffer>();
        Log::Info("SystemCommandBuffers: Initialized").enqueue();
    }

    void remove_system(){
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&on_shutdown>();
        Log::Info("SystemCommandBuffers: Removed").enqueue();
    }

    void on_startup(Events::Startup<Engine> &event){
        Log::Info("SystemCommandBuffers: Startup").enqueue();
    }

    void on_shutdown(Events::Shutdown<Engine> &event){
        Log::Info("SystemCommandBuffers: Shutdown").enqueue();
    }
}