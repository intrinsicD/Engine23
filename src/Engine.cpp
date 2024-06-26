//
// Created by alex on 22.11.23.
//

#include "Engine.h"
#include "Commands.h"
#include "systems/Systems.h"
#include "Events/Events.h"
#include "fmt/core.h"
#include "Input.h"
#include "Time.h"
#include "CommandDoubleBufferSimulation.h"
#include "CommandDoubleBufferRender.h"

namespace Bcg {

    Engine::Engine() {
        entt::locator<Engine *>::emplace<Engine *>(this);

        SystemTimer::pre_init();
        SystemCommandBuffers::pre_init();
        SystemPlatform::pre_init();
        SystemLogger::pre_init();
        SystemRendererOpenGL::pre_init();
        SystemWindowGLFW::pre_init();
        SystemGui::pre_init();
        SystemMouse::pre_init();
        SystemKeyboard::pre_init();
        SystemParallelProcessing::pre_init();
        SystemPlugins::pre_init();
        SystemCamera::pre_init();
        SystemEntity::pre_init();
        SystemPicker::pre_init();
        SystemTransform::pre_init();
        SystemTexture::pre_init();
        SystemHierarchy::pre_init();
        SystemAABB::pre_init();
        SystemMesh::pre_init();
        SystemAsset::pre_init();
        SystemActiveMaterials::pre_init();
        SystemAttribute::pre_init();
        SystemMaterial::pre_init();
        SystemUniform::pre_init();

        auto &time = Engine::Context().get<Time>();
        time.engine_constructor_start = Time::Point::Now();

        SystemTimer::init();
        SystemCommandBuffers::init();
        SystemPlatform::init();
        SystemLogger::init();
        SystemRendererOpenGL::init();
        SystemWindowGLFW::init();
        SystemGui::init();
        SystemMouse::init();
        SystemKeyboard::init();
        SystemParallelProcessing::init();
        SystemPlugins::init();
        SystemCamera::init();
        SystemEntity::init();
        SystemPicker::init();
        SystemTransform::init();
        SystemTexture::init();
        SystemHierarchy::init();
        SystemAABB::init();
        SystemMesh::init();
        SystemAsset::init();
        SystemActiveMaterials::init();
        SystemAttribute::init();
        SystemMaterial::init();
        SystemUniform::init();

        time.engine_constructor_end = Time::Point::Now();
        Log::Info(fmt::format("Engine: Constructor took {} seconds",
                              time.engine_constructor_end.duration<Time::Unit::seconds>(
                                      time.engine_constructor_start))).enqueue();
    }

    Engine::~Engine() {
        SystemUniform::remove();
        SystemMaterial::remove();
        SystemAttribute::remove();
        SystemActiveMaterials::remove();
        SystemAsset::remove();
        SystemMesh::remove();
        SystemAABB::remove();
        SystemHierarchy::remove();
        SystemTransform::remove();
        SystemTexture::remove();
        SystemEntity::remove();
        SystemPicker::remove();
        SystemCamera::remove();
        SystemPlugins::remove();
        SystemParallelProcessing::remove();
        SystemMouse::remove();
        SystemKeyboard::remove();
        SystemGui::remove();
        SystemRendererOpenGL::remove();
        SystemWindowGLFW::remove();
        SystemPlatform::remove();
        SystemTimer::remove();
        SystemLogger::remove();

        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();

        SystemCommandBuffers::remove();

        entt::locator<Engine *>::reset();
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
                dispatcher.trigger<Events::End<SimulationLoop>>();
                dispatcher.trigger<Events::Update<SimulationCommandDoubleBuffer>>();
            }

            dispatcher.trigger<Events::Begin<Frame>>();
            dispatcher.trigger<Events::Update<Frame>>();
            dispatcher.trigger<Events::Update<GuiMenu>>();
            dispatcher.trigger<Events::Update<Gui>>();
            dispatcher.trigger<Events::End<Frame>>();
            dispatcher.trigger<Events::Update<RenderCommandDoubleBuffer>>();
            dispatcher.trigger<Events::End<MainLoop>>();
            dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        }

        //shutdown engine
        Log::Info("Engine: Shutdown...").enqueue();
        dispatcher.trigger<Events::Shutdown<Engine>>();
        dispatcher.trigger<Events::Update<CommandDoubleBuffer>>();
        time.engine_run_end = Time::Point::Now();
    }
}