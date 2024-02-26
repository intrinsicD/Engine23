//
// Created by alex on 27.11.23.
//

#include "systems/SystemDearImGui.h"
#include "Engine.h"
#include "Events.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Commands.h"
#include "components/Window.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemGuiInternal {
        static bool show_demo_imgui = false;

        void on_render_gui(const Events::Render<Gui> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_begin_frame(const Events::Begin<Frame> &event);

        void on_end_frame(const Events::End<Frame> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemGuiInternal {
        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_demo_imgui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemGuiInternal::on_render_gui>();
                return;
            }
            ImGui::ShowDemoWindow(&show_demo_imgui);
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Show Demo Window", nullptr,
                                    &show_demo_imgui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemGuiInternal::on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_begin_frame(const Events::Begin<Frame> &event) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
            ImGui::BeginMainMenuBar();
        }

        void on_end_frame(const Events::End<Frame> &event) {
            ImGui::EndMainMenuBar();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemGuiInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::End<Frame>>().connect<&on_end_frame>();
            Log::Info(SystemGui::name() , "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemGuiInternal::on_render_gui_menu>();
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::End<Frame>>().disconnect<&on_end_frame>();
            Log::Info(SystemGui::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemGui::name() {
        return "SystemGui";
    }

    void SystemGui::pre_init() {

    }

    void SystemGui::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemGuiInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemGuiInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemGui::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemGuiInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemGuiInternal::on_shutdown_engine>();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        Log::Info("Removed", name()).enqueue();
    }

    void SystemGui::add_to_window(void *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();
        io.IniFilename = nullptr;

        auto &style = ImGui::GetStyle();
        style.WindowRounding = 0;
        //io.FontGlobalScale = Engine::Context().get<Window>().dpi;
        ImGui::StyleColorsDark(&style);
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), true);
#ifndef __APPLE__
        ImGui_ImplOpenGL3_Init();
#endif
        Log::Info(name() + ": Create DearImGui context").enqueue();
    }
}