//
// Created by alex on 27.11.23.
//

#define GLAD_GL_IMPLEMENTATION

#include "SystemRendererOpenGL.h"
#include "Engine.h"
#include "Events.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "imgui.h"

namespace Bcg {
    namespace SystemRendererOpenGLInternal {
        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemRendererOpenGLInternal::on_render_gui>();
                return;
            }

            if(ImGui::Begin("Renderer", &show_gui)) {
                auto &opengl_config = Engine::Context().get<OpenGLConfig>();
                ImGui::Text("OpenGL vendor: %s", opengl_config.vendor.c_str());
                ImGui::Text("OpenGL renderer: %s", opengl_config.renderer.c_str());
                ImGui::Text("OpenGL version: %s", opengl_config.version.c_str());
                ImGui::Text("OpenGL GLSL version: %s", opengl_config.glsl_version.c_str());
            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Renderer")) {
                if (ImGui::MenuItem("Info", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&SystemRendererOpenGLInternal::on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_begin_frame(const Events::Begin<Frame> &event) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            int version = gladLoadGL(glfwGetProcAddress);

            if (!version) {
                Log::Error("Failed to initialize OpenGL context").enqueue();
                return;
            }
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemRendererOpenGLInternal::on_render_gui_menu>();

            auto &opengl_config = Engine::Context().get<OpenGLConfig>();
            opengl_config.major = GLAD_VERSION_MAJOR(version);
            opengl_config.minor = GLAD_VERSION_MINOR(version);
            const auto &name = SystemRendererOpenGL().name();
            if (opengl_config.major < opengl_config.major_hint || opengl_config.minor < opengl_config.minor_hint) {
                Log::Error(name + ": OpenGL version not supported").enqueue();
                return;
            } else {
                opengl_config.vendor = (const char *) glGetString(GL_VENDOR);
                opengl_config.renderer = (const char *) glGetString(GL_RENDERER);
                opengl_config.version = (const char *) glGetString(GL_VERSION);
                opengl_config.glsl_version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
                Log::Info(name + ": Startup").enqueue();
                Log::Info(name + ": OpenGL vendor: " + opengl_config.vendor).enqueue();
                Log::Info(name + ": OpenGL renderer: " + opengl_config.renderer).enqueue();
                Log::Info(name + ": OpenGL version: " + opengl_config.version).enqueue();
                Log::Info(name + ": OpenGL GLSL version: " + opengl_config.glsl_version).enqueue();
            }
            auto &bg = Engine::Context().get<WindowConfig>().background_color;
            glClearColor(bg[0], bg[1], bg[2], bg[3]);
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemRendererOpenGLInternal::on_render_gui_menu>();
        }
    }

    SystemRendererOpenGL::SystemRendererOpenGL() : System("SystemRenderer") {

    }

    void SystemRendererOpenGL::pre_init() {
        Engine::Context().emplace<OpenGLConfig>();
    }

    void SystemRendererOpenGL::init() {
        auto &opengl_config = Engine::Context().get<OpenGLConfig>();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_config.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_config.minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemRendererOpenGLInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemRendererOpenGLInternal::on_shutdown_engine>();

        Log::Info(m_name + ": Initialized").enqueue();
    }

    void SystemRendererOpenGL::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemRendererOpenGLInternal::on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemRendererOpenGLInternal::on_shutdown_engine>();
        Log::Info(m_name + ": Removed").enqueue();
    }

}
