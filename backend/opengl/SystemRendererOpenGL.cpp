//
// Created by alex on 27.11.23.
//

#define GLAD_GL_IMPLEMENTATION

#include "SystemRendererOpenGL.h"
#include "Engine.h"
#include "Events.h"
#include "fmt/core.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "imgui.h"
#include "SystemShaderPrograms.h"
#include "SystemBuffers.h"

namespace Bcg {
    namespace SystemRendererOpenGLInternal {
        void on_update_viewport(const Events::Update<Viewport> &event) {
            auto &window_config = Engine::Context().get<WindowConfig>();
            SystemRendererOpenGL::set_viewport(window_config.width, window_config.height);
        }

        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&SystemRendererOpenGLInternal::on_render_gui>();
                return;
            }

            if (ImGui::Begin("Renderer", &show_gui)) {
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

        static std::shared_ptr<TaskCommand> forward_render;

        void on_render_frame(const Events::Render<Frame> &event) {
            auto &double_buffer = Engine::Context().get<RenderCommandDoubleBuffer>();
            double_buffer.enqueue_next(forward_render);
        }

        void on_startup_renderer(const Events::Startup<Renderer> &event) {
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::Update<Viewport>>().connect<&SystemRendererOpenGLInternal::on_update_viewport>();
            Engine::Instance()->dispatcher.sink<Events::Render<Frame>>().connect<&SystemRendererOpenGLInternal::on_render_frame>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&SystemRendererOpenGLInternal::on_render_gui_menu>();

            int version = gladLoadGL(glfwGetProcAddress);

            if (!version) {
                Log::Error("Failed to initialize OpenGL context").enqueue();
                return;
            }

            auto &opengl_config = Engine::Context().get<OpenGLConfig>();
            opengl_config.major = GLAD_VERSION_MAJOR(version);
            opengl_config.minor = GLAD_VERSION_MINOR(version);
            if (opengl_config.major < opengl_config.major_hint || opengl_config.minor < opengl_config.minor_hint) {
                Log::Error(fmt::format("{}: OpenGL version not supported", SystemRendererOpenGL::name())).enqueue();
                return;
            } else {
                opengl_config.vendor = (const char *) glGetString(GL_VENDOR);
                opengl_config.renderer = (const char *) glGetString(GL_RENDERER);
                opengl_config.version = (const char *) glGetString(GL_VERSION);
                opengl_config.glsl_version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
                Log::Info(SystemRendererOpenGL::name() + ": Startup").enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL vendor:       " + opengl_config.vendor).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL renderer:     " + opengl_config.renderer).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL version:      " + opengl_config.version).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL GLSL version: " +
                          opengl_config.glsl_version).enqueue();
            }

            auto &bg = Engine::Context().get<WindowConfig>().background_color;
            glClearColor(bg[0], bg[1], bg[2], bg[3]);

            forward_render = std::make_shared<TaskCommand>("Render", []() {
                auto &render_batches = Engine::Context().get<RenderBatches>();
                for (const auto &[shader_id, batch]: render_batches.batches) {
                    glUseProgram(shader_id);
                    batch->update_global_uniforms(shader_id);
                    for (const auto &renderable: batch->renderables) {
                        renderable->update_local_uniforms(shader_id);
                        renderable->draw();
                    }
                }
                return 1;
            });
        }

        void on_shutdown_renderer(const Events::Shutdown<Renderer> &event) {
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().disconnect<&SystemRendererOpenGLInternal::on_render_gui_menu>();
            Log::Info(SystemRendererOpenGL::name() + ": Shutdown").enqueue();
        }
    }

    void SystemRendererOpenGL::set_viewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
        OpenGL::AssertNoOglError();
    }

    void SystemRendererOpenGL::set_viewport(int width, int height) {
        set_viewport(0, 0, width, height);
    }

    std::string SystemRendererOpenGL::name() {
        return "SystemRenderer";
    }

    void SystemRendererOpenGL::pre_init() {
        Engine::Context().emplace<OpenGLConfig>();
        Engine::Context().emplace<RenderBatches>();
        SystemShaderPrograms().pre_init();
        SystemBuffers().pre_init();
    }

    void SystemRendererOpenGL::init() {
        auto &opengl_config = Engine::Context().get<OpenGLConfig>();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_config.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_config.minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        Engine::Instance()->dispatcher.sink<Events::Startup<Renderer>>().connect<&SystemRendererOpenGLInternal::on_startup_renderer>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Renderer>>().connect<&SystemRendererOpenGLInternal::on_shutdown_renderer>();

        Log::Info(name() + ": Initialized").enqueue();
        SystemShaderPrograms().init();
        SystemBuffers().init();
    }

    void SystemRendererOpenGL::remove() {
        SystemBuffers().remove();
        SystemShaderPrograms().remove();
        Engine::Instance()->dispatcher.sink < Events::Startup <
        Renderer >> ().disconnect<&SystemRendererOpenGLInternal::on_startup_renderer>();
        Engine::Instance()->dispatcher.sink < Events::Shutdown <
        Renderer >> ().disconnect<&SystemRendererOpenGLInternal::on_shutdown_renderer>();
        Log::Info(name() + ": Removed").enqueue();
    }

}
