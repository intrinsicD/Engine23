//
// Created by alex on 27.11.23.
//

#include "SystemDearImGui.h"
#include "Engine.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Commands.h"
#include "Components.h"


namespace Bcg::System::Gui {

    static bool show_demo_imgui = true;
    static entt::entity entity_menu;

    void add_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&on_shutdown_engine>();

        entity_menu = Engine::State().create();

        Engine::State().emplace<std::shared_ptr<GuiCommand>>(entity_menu,
                                                             std::make_shared<GuiCommand>("RenderMenu", [&]() {
                                                                 if (ImGui::BeginMenu("Menu")) {
                                                                     ImGui::MenuItem("Show Demo Window", nullptr,
                                                                                     &show_demo_imgui);
                                                                     ImGui::EndMenu();
                                                                 }
                                                                 return 1;
                                                             }));
    }

    void remove_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&on_shutdown_engine>();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void on_startup_engine(const Events::Startup<Engine> &event) {
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
        Engine::Instance()->dispatcher.sink<Events::End<Frame>>().connect<&on_end_frame>();
    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&on_begin_frame>();
        Engine::Instance()->dispatcher.sink<Events::End<Frame>>().disconnect<&on_end_frame>();
    }

    void on_begin_frame(const Events::Begin<Frame> &event) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::BeginMainMenuBar();

        auto &double_buffer = Engine::State().ctx().get<CommandDoubleBuffer>();
        double_buffer.current->emplace_back(Engine::State().get<std::shared_ptr<GuiCommand>>(entity_menu));

        if (show_demo_imgui) {
            double_buffer.current->emplace_back(std::make_shared<TaskCommand>("GuiCommand", [&]() {
                ImGui::ShowDemoWindow(&show_demo_imgui);
                return 1;
            }));
        }
    }

    void on_end_frame(const Events::End<Frame> &event) {
        ImGui::EndMainMenuBar();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void add_to_window(void *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();
        io.IniFilename = nullptr;

        auto &style = ImGui::GetStyle();
        style.WindowRounding = 0;
        io.FontGlobalScale = Engine::Context().get<WindowConfig>().dpi;
        ImGui::StyleColorsDark(&style);
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), true);
#ifndef __APPLE__
        ImGui_ImplOpenGL3_Init();
#endif
        Log::Info("SystemGui: Create DearImGui context");
    }
}