//
// Created by alex on 23.04.24.
//

#include "PluginOpenGLRenderer.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "OpenGLUtils.h"

namespace Bcg::PluginOpenGLRendererInternal {
    static bool show_gui = false;

    struct RendererContext {
        bool glfw_initialized = false;
        double dpi_scale = 1.0;
        GLFWwindow *glfWwindow = nullptr;
        std::string title = "BcgViewer";
        int width = 800;
        int height = 600;
        int swap_interval = 1;
        bool glfw_registered_callbacks = false;
    };

    void on_render_frame(const Events::Render<Frame> &event);

    void on_render_gui(const Events::Render<Gui> &event);

    void on_render_gui_menu(const Events::Render<GuiMenu> &event);

    void on_startup(const Events::Startup<Plugin> &event);

    void on_shutdown(const Events::Shutdown<Engine> &event);

    double compute_dpi_scale();
}

namespace Bcg::PluginOpenGLRendererInternal {
    void on_render_gui(const Events::Render<Gui> &event) {
        if (!show_gui) {
            Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&PluginOpenGLRendererInternal::on_render_gui>();
            return;
        }

        if (ImGui::Begin("Renderer", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {

        }
        ImGui::End();
    }

    void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
        if (ImGui::BeginMenu("Menu")) {

            if (ImGui::MenuItem("Renderer", nullptr, &show_gui)) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&PluginOpenGLRendererInternal::on_render_gui>();
            }

            ImGui::EndMenu();
        }
    }

    double compute_dpi_scale() {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (!monitor) {
            return 1.0; // Default scale factor
        }
        int widthMM, heightMM;
        glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
        if (widthMM == 0 || heightMM == 0) {
            return 1.0; // Default scale factor
        }

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (!mode) {
            return 1.0; // Default scale factor
        }

        int widthPx = mode->width;
        int heightPx = mode->height;

        // Calculate DPI
        double dpiX = static_cast<float>(widthPx) / (static_cast<float>(widthMM) / 25.4);
        double dpiY = static_cast<float>(heightPx) / (static_cast<float>(heightMM) / 25.4);

        // Average and normalize DPI to get scale factor
        // Here, 96 is the standard DPI value.
        double dpiScale = (dpiX + dpiY) / 2.0 / 96.0;
        return dpiScale;
    }
}

namespace Bcg {
    PluginOpenGLRenderer::PluginOpenGLRenderer() : Plugin("OpenGLRenderer") {

    }

    void PluginOpenGLRenderer::pre_init() {
        glfwSetErrorCallback([](int error, const char *description) {
            Log::Error("GLFW: Error(" + std::to_string(error) + "): " + std::string(description)).enqueue();
        });

        if (!glfwInit()) {
            Log::Error(name() + ": Failed to initialize GLFW").enqueue();
        } else {
            Log::Info(name() + ": Initialized GLFW").enqueue();
        }

        auto &context = Engine::Context().emplace<PluginOpenGLRendererInternal::RendererContext>();
        context.glfw_initialized = true;
        context.dpi_scale = PluginOpenGLRendererInternal::compute_dpi_scale();
    }

    void PluginOpenGLRenderer::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginOpenGLRendererInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&PluginOpenGLRendererInternal::on_shutdown>();


        auto &context = Engine::Context().get<PluginOpenGLRendererInternal::RendererContext>();
        context.glfWwindow = glfwCreateWindow(context.width, context.height, context.title.c_str(),
                                              nullptr, nullptr);
        glfwSetWindowUserPointer(context.glfWwindow, Engine::Instance());
        glfwMakeContextCurrent(context.glfWwindow);
        glfwSwapInterval(context.swap_interval);

        glfwSetWindowCloseCallback(context.glfWwindow, [](GLFWwindow *h_window) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->window_close_callback) {
                engine->window_close_callback();
            }
            engine->is_running = false;
        });

        glfwSetWindowSizeCallback(context.glfWwindow, [](GLFWwindow *h_window, int width, int height) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if(engine->window_size_callback){
                engine->window_size_callback();
            }
            glViewport(0, 0, width, height);
            OpenGL::AssertNoOglError();
            engine->dispatcher.trigger(Events::Update<Viewport>{});
        });

        glfwSetMouseButtonCallback(context.glfWwindow, [](GLFWwindow *h_window, int button, int action, int mods) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if(engine->mouse_button_callback){
                engine->mouse_button_callback();
            }

            auto &input = engine->state.ctx().get<Input>();
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                input.mouse.button.left = action == GLFW_PRESS;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                input.mouse.button.right = action == GLFW_PRESS;
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                input.mouse.button.middle = action == GLFW_PRESS;
            }
            engine->dispatcher.trigger(Events::Update<Input::Mouse::Button>{});
        });

        Log::Info("Initialized", name()).enqueue();
    }

    void PluginOpenGLRenderer::remove() {
        glfwTerminate();

        Log::Info("Removed", name()).enqueue();
    }
}