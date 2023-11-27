//
// Created by alex on 27.11.23.
//

#include "SystemGui.h"
#include "Engine.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Commands.h"

namespace Bcg::System::Gui {
    void add_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&on_shutdown_engine>();
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
        Engine::Instance()->dispatcher.sink<Events::End<Frame>>().connect<&on_end_frame>();
    }

    void remove_system() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&on_shutdown_engine>();
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&on_begin_frame>();
        Engine::Instance()->dispatcher.sink<Events::End<Frame>>().disconnect<&on_end_frame>();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void on_startup_engine(const Events::Startup<Engine> &event) {

    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {

    }

    void on_begin_frame(const Events::Begin<Frame> &event) {

    }

    static bool show_demo_window = true;

    void on_end_frame(const Events::End<Frame> &event) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(&show_demo_window);

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
        ImGui::StyleColorsDark(&style);
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), true);
#ifndef __APPLE__
        ImGui_ImplOpenGL3_Init();
#endif
        Log::Info("SystemGui: Create context");
    }
}