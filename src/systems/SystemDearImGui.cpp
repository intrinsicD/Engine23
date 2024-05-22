//
// Created by alex on 27.11.23.
//

#include "systems/SystemDearImGui.h"
#include "Engine.h"
#include "Events/Events.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Commands.h"
#include "Window.h"
#include "Components.h"
#include "SystemWindowGLFW.h"
#include "GLFW/glfw3.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg{
    namespace SystemGuiInternal {
        static bool show_demo_imgui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

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
        float GetWindowDpiScale(GLFWwindow* window) {
            int windowWidth, windowHeight;
            int framebufferWidth, framebufferHeight;

            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

            float dpiScale = (float)framebufferWidth / windowWidth;
            return dpiScale;
        }

        void UpdateDpiScale(ImGuiIO& io, float dpiScale) {
            io.FontGlobalScale = dpiScale;
        }

        void LoadFonts(ImGuiIO& io, float dpiScale) {
            io.Fonts->Clear();
            io.Fonts->AddFontFromFileTTF("../ext/imgui/misc/fonts/ProggyClean.ttf", 16.0f * dpiScale);
            io.Fonts->Build();
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_demo_imgui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemGuiInternal::on_update_gui>();
                return;
            }
            ImGui::ShowDemoWindow(&show_demo_imgui);
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Show Demo Window", nullptr,
                                    &show_demo_imgui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemGuiInternal::on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_begin_frame(const Events::Begin<Frame> &event) {
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);

            ImGuiIO& io = ImGui::GetIO();
            //float dpiScale = GetWindowDpiScale(static_cast<GLFWwindow *>(window.window_handle));
            float dpiScale = window.dpi * 0.75;
            if (dpiScale != io.FontGlobalScale) {
                UpdateDpiScale(io, dpiScale);
                LoadFonts(io, dpiScale);
            }

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

            // Update and Render additional Platform Windows
            auto &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemGuiInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Begin<Frame>>().connect<&on_begin_frame>();
            Engine::Dispatcher().sink<Events::End<Frame>>().connect<&on_end_frame>();
            Log::Info(SystemGui::name() , "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemGuiInternal::on_update_gui_menu>();
            Engine::Dispatcher().sink<Events::Begin<Frame>>().disconnect<&on_begin_frame>();
            Engine::Dispatcher().sink<Events::End<Frame>>().disconnect<&on_end_frame>();
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
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemGuiInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemGuiInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemGui::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemGuiInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemGuiInternal::on_shutdown_engine>();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        Log::Info("Removed", name()).enqueue();
    }

    void SystemGui::add_to_window(void *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.IniFilename = nullptr;



        auto &style = ImGui::GetStyle();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        //io.FontGlobalScale = Engine::Context().get<Window>().dpi;
        ImGui::StyleColorsDark(&style);
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), true);
#ifndef __APPLE__
        ImGui_ImplOpenGL3_Init();
#endif
        Log::Info(name() + ": Create DearImGui context").enqueue();
    }
}