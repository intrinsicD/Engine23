//
// Created by alex on 23.04.24.
//

#include "PluginOpenGLRenderer.h"
#include "Engine.h"
#include "Events/Events.h"
#include "InternalCallbackEvents.h"
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

    void on_update_frame(const Events::Update<Frame> &event);

    void on_update_gui(const Events::Update<Gui> &event);

    void on_update_gui_menu(const Events::Update<GuiMenu> &event);

    void on_callback_window_size(const Events::Callback::WindowSize &event);

    void on_startup(const Events::Startup<Plugin> &event);

    void on_shutdown(const Events::Shutdown<Engine> &event);

    double compute_dpi_scale();
}

namespace Bcg::PluginOpenGLRendererInternal {
    void on_update_gui(const Events::Update<Gui> &event) {
        if (!show_gui) {
            Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().disconnect<&PluginOpenGLRendererInternal::on_update_gui>();
            return;
        }

        if (ImGui::Begin("Renderer", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
            auto &context = Engine::Context().get<RendererContext>();
            ImGui::Text("GLFW initialized:          %d", context.glfw_initialized);
            ImGui::Text("GLFW registered callbacks: %d", context.glfw_registered_callbacks);
            ImGui::Text("DPI Scale:                 %.2f", context.dpi_scale);
            ImGui::Text("Window Size:               %d x %d", context.width, context.height);
            ImGui::Text("Swap Interval:             %d", context.swap_interval);
            ImGui::Text("Title:                     %s", context.title.c_str());
        }
        ImGui::End();
    }

    void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
        if (ImGui::BeginMenu("Menu")) {

            if (ImGui::MenuItem("Renderer", nullptr, &show_gui)) {
                Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().connect<&PluginOpenGLRendererInternal::on_update_gui>();
            }

            ImGui::EndMenu();
        }
    }

    void on_callback_window_size(const Events::Callback::WindowSize &event) {
        glViewport(0, 0, event.width, event.height);
        OpenGL::AssertNoOglError();
    }

    void on_startup(const Events::Startup<Plugin> &event) {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<PluginOpenGLRendererInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Callback::WindowSize>().connect<PluginOpenGLRendererInternal::on_callback_window_size>();

        Log::Info(PluginOpenGLRenderer::name(), "Startup").enqueue();
    }

    void on_shutdown(const Events::Shutdown<Engine> &event) {
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<PluginOpenGLRendererInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Callback::WindowSize>().disconnect<PluginOpenGLRendererInternal::on_callback_window_size>();

        Log::Info(PluginOpenGLRenderer::name(), "Shutdown").enqueue();
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

    std::string PluginOpenGLRenderer::name() {
        return "OpenGLRenderer";
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
            if (engine->window_size_callback) {
                engine->window_size_callback();
            }
            engine->dispatcher.trigger(Events::Callback::WindowSize{h_window, width, height});
        });

        glfwSetMouseButtonCallback(context.glfWwindow, [](GLFWwindow *h_window, int button, int action, int mods) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->mouse_button_callback) {
                engine->mouse_button_callback();
            }
            engine->dispatcher.trigger(Events::Callback::MouseButton{0, button, action, mods});
        });

        glfwSetCursorPosCallback(context.glfWwindow, [](GLFWwindow *h_window, double xpos, double ypos) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->cursor_pos_callback) {
                engine->cursor_pos_callback();
            }
            engine->dispatcher.trigger(Events::Callback::MouseCursorPosition{0, float(xpos), float(ypos)});
        });

        glfwSetScrollCallback(context.glfWwindow, [](GLFWwindow *h_window, double xoffset, double yoffset) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->scroll_callback) {
                engine->scroll_callback();
            }
            engine->dispatcher.trigger(Events::Callback::MouseScroll{h_window, float(xoffset), float(yoffset)});
        });

        glfwSetKeyCallback(context.glfWwindow, [](GLFWwindow *h_window, int key, int scancode, int action, int mods) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->key_callback) {
                engine->key_callback();
            }
            engine->dispatcher.trigger(Events::Callback::Key{h_window, scancode, action, mods});
        });

        glfwSetDropCallback(context.glfWwindow, [](GLFWwindow *h_window, int count, const char **paths) {
            auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(h_window));
            if (engine->drop_callback) {
                engine->drop_callback();
            }
            //Figure out how to handle file drop...
            //Option 1: Systems or plugins register to this callback and process all paths
            //Option 2: Preprocess to determine what files were dropped and how to continue with them...
            engine->dispatcher.trigger(Events::Callback::Drop{h_window, count, paths});
        });

        Log::Info("Initialized", name()).enqueue();
    }

    void PluginOpenGLRenderer::remove() {
        glfwTerminate();

        Log::Info("Removed", name()).enqueue();
    }
}