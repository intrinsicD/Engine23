//
// Created by alex on 01.12.23.
//

#include "SystemCommandBuffers.h"
#include "Events.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemCommandBuffersInternal{
        void on_update_simulation_command_double_buffer(const Events::Update<SimulationCommandDoubleBuffer> &event);

        void on_update_render_command_double_buffer(const Events::Update<RenderCommandDoubleBuffer> &event);

        void on_update_command_double_buffer(const Events::Update<CommandDoubleBuffer> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemCommandBuffersInternal{
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

        void on_startup(const Events::Startup<Engine> &event){
            Log::Info(SystemCommandBuffers::name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event){
            Log::Info(SystemCommandBuffers::name() + ": Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg{
    std::string SystemCommandBuffers::name(){
        return "SystemCommandBuffers";
    }

    void SystemCommandBuffers::pre_init(){
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

    void SystemCommandBuffers::init(){
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemCommandBuffersInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemCommandBuffersInternal::on_shutdown>();
        Engine::Instance()->dispatcher.sink<Events::Update<SimulationCommandDoubleBuffer>>().connect<&SystemCommandBuffersInternal::on_update_simulation_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<RenderCommandDoubleBuffer>>().connect<&SystemCommandBuffersInternal::on_update_render_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<CommandDoubleBuffer>>().connect<&SystemCommandBuffersInternal::on_update_command_double_buffer>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemCommandBuffers::remove(){
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemCommandBuffersInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemCommandBuffersInternal::on_shutdown>();
        Engine::Instance()->dispatcher.sink<Events::Update<SimulationCommandDoubleBuffer>>().disconnect<&SystemCommandBuffersInternal::on_update_simulation_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<RenderCommandDoubleBuffer>>().disconnect<&SystemCommandBuffersInternal::on_update_render_command_double_buffer>();
        Engine::Instance()->dispatcher.sink<Events::Update<CommandDoubleBuffer>>().disconnect<&SystemCommandBuffersInternal::on_update_command_double_buffer>();
        Log::Info(name() + ": Removed").enqueue();
    }
}