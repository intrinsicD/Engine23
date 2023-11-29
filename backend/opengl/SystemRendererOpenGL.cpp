//
// Created by alex on 27.11.23.
//

#define GLAD_GL_IMPLEMENTATION

#include "SystemRenderer.h"
#include "Engine.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Commands.h"
#include "Components.h"
#include "imgui.h"


namespace Bcg::System::Renderer {
    void add_system() {
        auto *opengl_config = Engine::Context().find<OpenGLConfig>();
        if (!opengl_config) {
/*            Log::Error("SystemRenderer: OpenGLConfig not found").enqueue();*/
            opengl_config = &Engine::Context().emplace<OpenGLConfig>();
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_config->major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_config->minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<on_startup_engine>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<on_shutdown_engine>();
        Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<on_begin_frame>();
        Engine::Instance()->dispatcher.sink<Events::End<Frame>>().connect<on_end_frame>();
    }

    void remove_system() {

    }

    void on_startup_engine(const Events::Startup<Engine> &event) {
        int version = gladLoadGL(glfwGetProcAddress);

        if (!version) {
            Log::Error("Failed to initialize OpenGL context").enqueue();
            return;
        }

        auto &opengl_config = Engine::Context().get<OpenGLConfig>();
        opengl_config.major = GLAD_VERSION_MAJOR(version);;
        opengl_config.minor = GLAD_VERSION_MINOR(version);
        if (opengl_config.major < opengl_config.major_hint || opengl_config.minor < opengl_config.minor_hint) {
            Log::Error("SystemRenderer: OpenGL version not supported").enqueue();
            return;
        } else {
            opengl_config.vendor = (const char *) glGetString(GL_VENDOR);
            opengl_config.renderer = (const char *) glGetString(GL_RENDERER);
            opengl_config.version = (const char *) glGetString(GL_VERSION);
            opengl_config.glsl_version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
            Log::Info("SystemRenderer: OpenGL vendor: " + opengl_config.vendor).enqueue();
            Log::Info("SystemRenderer: OpenGL renderer: " + opengl_config.renderer).enqueue();
            Log::Info("SystemRenderer: OpenGL version: " + opengl_config.version).enqueue();
            Log::Info("SystemRenderer: OpenGL GLSL version: " + opengl_config.glsl_version).enqueue();
        }
        auto &bg = Engine::Context().get<WindowConfig>().background_color;
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
    }

    void on_shutdown_engine(const Events::Shutdown<Engine> &event) {

    }

    void on_begin_frame(const Events::Begin<Frame> &event) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void on_end_frame(const Events::End<Frame> &event) {

    }
}
