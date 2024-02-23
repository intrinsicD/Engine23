//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "systems/Systems.h"
#include "Events.h"
#include "fmt/core.h"
#include "components/Input.h"
#include "components/Time.h"
#include "components/CommandDoubleBufferSimulation.h"
#include "components/CommandDoubleBufferRender.h"

namespace Bcg {
    Engine::Engine() {
        //The engine is a singleton
        //Has an entt::registry as state to store and represent the current state of the engine or application
        //Has an entt::dispatcher as dispatcher for decoupled communication between parts of the engine or application

        //The engine has a set of commands that are executed in a specific order
        //The command buffer is double buffered. It always contains the following commands:
        //ProcessUserCommands, UpdateStateCommand, RenderCommand, PostRenderCommand in that order.
        //Each command is Composite, it can contain other commands. This should be flexible enough for most cases, but can be of course extended if needed.

        //The main way to interact with the engine is via commands and events over the dispatcher or via direct state access.
        //The main way to extend the engines functionality is via plugins, systems and managers. But of course the engine can be extended in any way.

        entt::locator<Engine *>::emplace<Engine *>(this);

        SystemTimer::pre_init();
        SystemCommandBuffers::pre_init();
        SystemPlatform::pre_init();
        SystemLogger::pre_init();
        SystemRendererOpenGL::pre_init();
        SystemWindowGLFW::pre_init();
        SystemGui::pre_init();
        SystemUserInput::pre_init();
        SystemParallelProcessing::pre_init();
        SystemPlugins::pre_init();
        SystemCamera::pre_init();
        SystemEntity::pre_init();
        SystemTransform::pre_init();
        SystemTriMesh::pre_init();
        SystemHierarchy::pre_init();
        SystemAABB::pre_init();
        SystemMesh::pre_init();

        auto &time = Engine::Context().get<Time>();
        time.engine_constructor_start = Time::Point::Now();

        SystemTimer::init();
        SystemCommandBuffers::init();
        SystemPlatform::init();
        SystemLogger::init();
        SystemRendererOpenGL::init();
        SystemWindowGLFW::init();
        SystemGui::init();
        SystemUserInput::init();
        SystemParallelProcessing::init();
        SystemPlugins::init();
        SystemCamera::init();
        SystemEntity::init();
        SystemTransform::init();
        SystemTriMesh::init();
        SystemHierarchy::init();
        SystemAABB::init();
        SystemMesh::init();

        time.engine_constructor_end = Time::Point::Now();
        Log::Info(fmt::format("Engine: Constructor took {} seconds",
                              time.engine_constructor_end.duration<Time::Unit::seconds>(
                                      time.engine_constructor_start))).enqueue();
    }

    Engine::~Engine() {
/*
        SystemHierarchy::remove();
        SystemTriMesh::remove();
        SystemTransform::remove();
        SystemEntity::remove();
        SystemCamera::remove();
        SystemPlugins::remove();
        SystemParallelProcessing::remove();
        SystemUserInput::remove();
        SystemGui::remove();
        SystemRendererOpenGL::remove();
        SystemWindowGLFW::remove();
        SystemPlatform::remove();
        SystemTimer::remove();
        SystemLogger::remove();

        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();

        SystemCommandBuffers::remove();

        entt::locator<Engine *>::reset();*/
    }

    void Engine::run() {
        auto &time = Engine::Context().get<Time>();
        time.engine_run_start = Time::Point::Now();
        is_running = true;
        //The primary workflow is this:
        //1. notify all systems of an event
        //2. systems can add commands to the command buffer
        //3. execute the command buffer
        //4. repeat

        //startup engine
        Log::Info("Engine: Startup...").enqueue();
        dispatcher.trigger<Events::Startup<Engine>>();
        dispatcher.trigger<Events::Startup<Plugin>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();

        Log::Info("Engine: Run...").enqueue();
        time.mainloop.current = Time::Point::Now();
        time.simulationloop.avg_duration = time.simulationloop.min_step_size;
        time.simulationloop.duration = time.simulationloop.min_step_size;

        while (is_running) {
            dispatcher.trigger<Events::Begin<MainLoop>>();
            dispatcher.trigger<Events::Update<Input>>();

            while (time.simulationloop.accumulator > time.simulationloop.duration) {
                dispatcher.trigger<Events::Begin<SimulationLoop>>();
                dispatcher.trigger<Events::Update<SimulationLoop>>();
                dispatcher.trigger<Events::Update<SimulationCommandDoubleBuffer>>();
                dispatcher.trigger<Events::End<SimulationLoop>>();
            }

            dispatcher.trigger<Events::Begin<Frame>>();
            dispatcher.trigger<Events::Render<Frame>>();
            dispatcher.trigger<Events::Render<GuiMenu>>();
            dispatcher.trigger<Events::Render<Gui>>();
            dispatcher.trigger<Events::Update<RenderCommandDoubleBuffer>>();
            dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
            dispatcher.trigger<Events::End<Frame>>();
            dispatcher.trigger<Events::End<MainLoop>>();
        }

        //shutdown engine
        Log::Info("Engine: Shutdown...").enqueue();
        dispatcher.trigger<Events::Shutdown<Engine>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        time.engine_run_end = Time::Point::Now();
    }
}