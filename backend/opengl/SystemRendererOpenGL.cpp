//
// Created by alex on 27.11.23.
//

#define GLAD_GL_IMPLEMENTATION

#include "SystemRendererOpenGL.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Events/InternalCallbackEvents.h"
#include "fmt/core.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "imgui.h"
#include "SystemShaderPrograms.h"
#include "SystemBuffers.h"
#include "Window.h"
#include "Viewport.h"
#include "OpenGLInfo.h"
#include "CommandDoubleBufferRender.h"
#include "Renderable.h"
#include "Picker.h"
#include "OpenGLUtils.h"
#include "GLMeshRenderPass.h"
#include "Components.h"
#include "SystemWindowGLFW.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemRendererOpenGLInternal {
        static bool show_gui = false;
        static bool show_gui_renderable_triangles = false;

        void on_update_viewport(const Events::Update<Viewport> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_begin_frame(const Events::Begin<Frame> &event);

        static std::shared_ptr<TaskCommand> forward_render;

        void on_update_frame(const Events::Update<Frame> &event);

        void on_startup_renderer(const Events::Startup<Renderer> &event);

        void on_shutdown_renderer(const Events::Shutdown<Renderer> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemRendererOpenGLInternal {
        void on_update_viewport(const Events::Update<Viewport> &event) {
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();
            auto &window = windows.get_instance(component_window);

            auto size = window.get_size();
            SystemRendererOpenGL::set_viewport(size[0], size[1]);
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemRendererOpenGLInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Renderer", &show_gui)) {
                auto &opengl_config = Engine::Context().get<OpenglInfo>();
                ImGui::Text("OpenGL vendor: %s", opengl_config.vendor.c_str());
                ImGui::Text("OpenGL renderer: %s", opengl_config.renderer.c_str());
                ImGui::Text("OpenGL version: %s", opengl_config.version.c_str());
                ImGui::Text("OpenGL GLSL version: %s", opengl_config.glsl_version.c_str());
            }
            ImGui::End();
        }

        void on_update_gui_renderable_triangles(const Events::Update<Gui> &event) {
            if (!show_gui_renderable_triangles) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemRendererOpenGLInternal::on_update_gui_renderable_triangles>();
                return;
            }

            auto &picker = Engine::Context().get<Picker>();
            if (ImGui::Begin("RenderableTriangles", &show_gui_renderable_triangles)) {
                if (picker.id.entity != entt::null) {
                    auto &renderable = Engine::State().get<OpenGL::RenderableTriangles>(picker.id.entity);
                    ComponentGui<OpenGL::RenderableTriangles>::Show(renderable);
                }
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Renderer")) {
                if (ImGui::MenuItem("Info", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemRendererOpenGLInternal::on_update_gui>();
                }
                if (ImGui::MenuItem("RenderableTriangles", nullptr, &show_gui_renderable_triangles)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemRendererOpenGLInternal::on_update_gui_renderable_triangles>();
                }
                ImGui::EndMenu();
            }
        }

        void on_begin_frame(const Events::Begin<Frame> &event) {
            Components<Window> windows(SystemWindowGLFW::component_name());
            auto &component_window = Engine::Context().get<Component<Window>>();

            for (auto window_index: windows.container.used_list) {
                auto &window = windows.get_instance(window_index);
                auto &bg = window.background_color;
                glfwMakeContextCurrent(static_cast<GLFWwindow *>(window.window_handle));

                glClearColor(bg[0], bg[1], bg[2], bg[3]);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            auto view = Engine::State().view<RenderCommand>();
            for (auto entity_id: view) {
                auto &render_command = view.get<RenderCommand>(entity_id);
                auto &double_buffer = Engine::Context().get<RenderCommandDoubleBuffer>();
                double_buffer.enqueue_next(std::make_shared<RenderCommand>(render_command));
            }
        }

        void on_update_frame(const Events::Update<Frame> &event) {
            GLMeshRenderPass meshes;
            meshes.execute();
        }

        void on_startup_renderer(const Events::Startup<Renderer> &event) {
            Engine::Dispatcher().sink<Events::Begin<Frame>>().connect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Dispatcher().sink<Events::Update<Viewport>>().connect<&SystemRendererOpenGLInternal::on_update_viewport>();
            Engine::Dispatcher().sink<Events::Update<Frame>>().connect<&SystemRendererOpenGLInternal::on_update_frame>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemRendererOpenGLInternal::on_update_gui_menu>();

            int version = gladLoadGL(glfwGetProcAddress);

            if (!version) {
                Log::Error("Failed to initialize OpenGL context").enqueue();
                return;
            }

            auto &opengl_config = Engine::Context().get<OpenglInfo>();
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
                Log::Info(SystemRendererOpenGL::name(), "Startup").enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL vendor:       " + opengl_config.vendor).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL renderer:     " + opengl_config.renderer).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL version:      " + opengl_config.version).enqueue();
                Log::Info(SystemRendererOpenGL::name() + ": OpenGL GLSL version: " +
                          opengl_config.glsl_version).enqueue();
            }

            auto new_state = OpenGL::State();
            new_state.depth_test.enabled = true;
            new_state.depth_test.func = GL_LESS;

            auto &bg = Engine::Context().get<Window>().background_color;
            new_state.clear_color = {bg[0], bg[1], bg[2], bg[3]};

            auto &state = Engine::Context().emplace<OpenGL::State>();
            state.change_to_new_state(new_state);

            Log::Info(SystemRendererOpenGL::name(), "Startup").enqueue();
        }

        void on_shutdown_renderer(const Events::Shutdown<Renderer> &event) {
            Engine::Dispatcher().sink<Events::Begin<Frame>>().disconnect<&SystemRendererOpenGLInternal::on_begin_frame>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemRendererOpenGLInternal::on_update_gui_menu>();
            Log::Info(SystemRendererOpenGL::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    void SystemRendererOpenGL::set_viewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
        OpenGL::AssertNoOglError();
    }

    void SystemRendererOpenGL::set_viewport(int width, int height) {
        set_viewport(0, 0, width, height);
    }

    void *SystemRendererOpenGL::create_window(int width, int height, const std::string &title) {
        return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    void SystemRendererOpenGL::destroy(void *window_handle) {
        glfwDestroyWindow(static_cast<GLFWwindow *>(window_handle));
    }

    void SystemRendererOpenGL::make_current(void *window_handle) {
        glfwMakeContextCurrent(static_cast<GLFWwindow *>(window_handle));
    }

    void SystemRendererOpenGL::swap_buffers(void *window_handle) {
        glfwSwapBuffers(static_cast<GLFWwindow *>(window_handle));
    }

    void SystemRendererOpenGL::set_window_size(void *window_handle, int width, int height) {
        glfwSetWindowSize(static_cast<GLFWwindow *>(window_handle), width, height);
    }

    double SystemRendererOpenGL::get_dpi_for_monitor(void *monitor) {
        if (!monitor) {
            return 1.0; // Default scale factor
        }

        GLFWmonitor *glfWmonitor = static_cast<GLFWmonitor *>(monitor);

        int width, height;
        glfwGetMonitorPhysicalSize(glfWmonitor, &width, &height);
        if (width == 0 || height == 0) {
            return 1.0; // Default scale factor
        }

        // Get monitor resolution
        const GLFWvidmode *mode = glfwGetVideoMode(glfWmonitor);
        if (!mode) {
            return 1.0; // Default scale factor
        }

        // Calculate DPI
        double dpiX = static_cast<double>(mode->width) / (static_cast<double>(width) / 25.4);
        double dpiY = static_cast<double>(mode->height) / (static_cast<double>(height) / 25.4);

        // Average and normalize DPI to get scale factor
        // Here, 96 is the standard DPI value.
        double dpiScale = (dpiX + dpiY) / 2.0 / 96.0;

        return dpiScale;
    }

    float SystemRendererOpenGL::get_depth_at_screen_position(float x, float y){
        Components<Window> windows(SystemWindowGLFW::component_name());
        auto &component_window = Engine::Context().get<Component<Window>>();
        auto &window = windows.get_instance(component_window);
        auto size = window.get_size();

        // Convert screen position to window coordinates
        // OpenGL's origin is at the bottom-left corner

        // Read the depth value at the specified window coordinates
        float depth;
        glReadPixels(x, size[1] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

        return depth;
    }

    std::string SystemRendererOpenGL::name() {
        return "SystemRenderer";
    }

    void SystemRendererOpenGL::pre_init() {
        Engine::Context().emplace<OpenglInfo>();
        Engine::Context().emplace<RenderBatches>();
        SystemShaderPrograms().pre_init();
        SystemBuffers().pre_init();
    }

    void SystemRendererOpenGL::init() {
        auto &opengl_config = Engine::Context().get<OpenglInfo>();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_config.major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_config.minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        Engine::Dispatcher().sink<Events::Startup<Renderer>>().connect<&SystemRendererOpenGLInternal::on_startup_renderer>();
        Engine::Dispatcher().sink<Events::Shutdown<Renderer>>().connect<&SystemRendererOpenGLInternal::on_shutdown_renderer>();

        Log::Info("Initialized", name()).enqueue();
        SystemShaderPrograms().init();
        SystemBuffers().init();
    }

    void SystemRendererOpenGL::remove() {
        SystemBuffers().remove();
        SystemShaderPrograms().remove();
        Engine::Dispatcher().sink<Events::Startup<Renderer >>().disconnect<&SystemRendererOpenGLInternal::on_startup_renderer>();
        Engine::Dispatcher().sink<Events::Shutdown<
                Renderer >>().disconnect<&SystemRendererOpenGLInternal::on_shutdown_renderer>();
        Log::Info("Removed", name()).enqueue();
    }

}
