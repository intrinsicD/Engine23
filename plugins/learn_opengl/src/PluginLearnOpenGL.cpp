//
// Created by alex on 07.12.23.
//

#include "PluginLearnOpenGL.h"
#include "OpenGLUtils.h"
#include "Engine.h"
#include "Events.h"
#include "Components.h"
#include "Commands.h"

namespace Bcg {
    namespace PluginLearnOpenGLInternal {
        void on_render_frame(const Events::Render<Frame> &event) {
            auto view = Engine::State().view<OpenGL::Renderable>();
            for (const auto &entity_id: view) {
                auto &renderable = view.get<OpenGL::Renderable>(entity_id);
                renderable.draw();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<Frame>>().connect<&on_render_frame>();


            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            OpenGL::ShaderProgram program;
            program.name = "learn_opengl";
            program.v_shader.source = "#version 330 core\n"
                                      "layout (location = 0) in vec3 aPos;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                      "}\0";
            program.f_shader.source = "#version 330 core\n"
                                      "out vec4 FragColor;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                      "}\n\0";
            program.compile_from_sources();
            program.link();
            if (program.check_link_status()) {
                programs[program.name] = program;
            } else {
                auto m_name = PluginLearnOpenGL().name();
                Log::Error(m_name + ": " + program.name + " Error: " + program.error_message).enqueue();
            }

            float vertices[] = {
                    0.5f, 0.5f, 0.0f,  // top right
                    0.5f, -0.5f, 0.0f,  // bottom right
                    -0.5f, -0.5f, 0.0f,  // bottom left
                    -0.5f, 0.5f, 0.0f   // top left
            };
            unsigned int indices[] = {  // note that we start from 0!
                    0, 1, 3,  // first Triangle
                    1, 2, 3   // second Triangle
            };

            auto entity_id = Engine::State().create();
            auto &renderable = Engine::State().emplace<OpenGL::Renderable>(entity_id);
            renderable = OpenGL::Renderable::Triangles();
            renderable.vbo = OpenGL::VertexBufferObject::Static();
            renderable.ebo = OpenGL::IndexBufferObject::Static();

            renderable.vao.create();
            renderable.vbo.create();
            renderable.ebo.create();

            renderable.vao.bind();
            renderable.vbo.bind();
            renderable.vbo.set_data(vertices, sizeof(vertices));

            renderable.ebo.bind();
            renderable.ebo.set_data(indices, sizeof(indices));

            renderable.vao.set_float_attribute(0, 3, false, (void *) 0);

            renderable.vbo.release();
            renderable.vao.release();
            renderable.ebo.release();
            renderable.program = programs["learn_opengl"];
            renderable.count = 6;
            renderable.offset = 0;
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<Frame>>().disconnect<&on_render_frame>();
        }
    }

    PluginLearnOpenGL::PluginLearnOpenGL() : Plugin("learn_opengl") {

    }

    void PluginLearnOpenGL::pre_init() {

    }

    void PluginLearnOpenGL::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginLearnOpenGLInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&PluginLearnOpenGLInternal::on_shutdown>();

        Log::Info(m_name + ": Initialized").enqueue();
    }

    void PluginLearnOpenGL::remove() {
        Log::Info(m_name + ": Removed").enqueue();
    }
}