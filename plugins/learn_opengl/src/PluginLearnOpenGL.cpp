//
// Created by alex on 07.12.23.
//

#include "PluginLearnOpenGL.h"
#include "OpenGLUtils.h"
#include "Engine.h"
#include "Events.h"
#include "Components.h"
#include "Commands.h"
#include "imgui.h"

namespace Bcg {
    namespace PluginLearnOpenGLInternal {
        void on_render_frame(const Events::Render<Frame> &event) {
            auto view = Engine::State().view<OpenGL::Renderable>();
            for (const auto &entity_id: view) {
                auto &renderable = view.get<OpenGL::Renderable>(entity_id);
                renderable.draw();
            }
        }

        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if(!show_gui){
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&PluginLearnOpenGLInternal::on_render_gui>();
                return;
            }

            static bool hello_triangle = false;
            static entt::entity hello_triangle_id;
            if(ImGui::Begin("Learn OpenGL", &show_gui)){
                if(ImGui::Checkbox("Hello Triangle", &hello_triangle)){
                    if(hello_triangle) {
                        std::array<float, 12> vertices{
                                0.5f, 0.5f, 0.0f,  // top right
                                0.5f, -0.5f, 0.0f,  // bottom right
                                -0.5f, -0.5f, 0.0f,  // bottom left
                                -0.5f, 0.5f, 0.0f   // top left
                        };

                        std::array<unsigned int, 6> indices = {  // note that we start from 0!
                                0, 1, 3,  // first Triangle
                                1, 2, 3   // second Triangle
                        };

                        hello_triangle_id = Engine::State().create();
                        auto &renderable = Engine::State().emplace<OpenGL::Renderable>(hello_triangle_id);
                        renderable = OpenGL::Renderable::Triangles();
                        renderable.vbo = OpenGL::VertexBufferObject::Static();
                        renderable.ebo = OpenGL::IndexBufferObject::Static();

                        renderable.vao.create();
                        renderable.vbo.create();
                        renderable.ebo.create();

                        renderable.vao.bind();
                        renderable.vbo.bind();
                        renderable.vbo.set_data(vertices.data(), vertices.size() * sizeof(vertices[0]));

                        renderable.ebo.bind();
                        renderable.ebo.set_data(indices.data(), indices.size() * sizeof(indices[0]));

                        renderable.vao.set_float_attribute(0, 3, false, (void *) 0);

                        renderable.vbo.release();
                        renderable.vao.release();
                        renderable.ebo.release();
                        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                        renderable.program = programs["learn_opengl"];
                        renderable.count = 6;
                        renderable.offset = 0;
                    }else{
                        auto &renderable = Engine::State().get<OpenGL::Renderable>(hello_triangle_id);
                        renderable.vao.destroy();
                        renderable.vbo.destroy();
                        renderable.ebo.destroy();
                        Engine::State().destroy(hello_triangle_id);
                    }
                }
            }
            ImGui::End();
        }

        void on_render_gui_menu(const Events::Render<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Learn OpenGL", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().connect<&PluginLearnOpenGLInternal::on_render_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<Frame>>().connect<&on_render_frame>();
            Engine::Instance()->dispatcher.sink<Events::Render<GuiMenu>>().connect<&on_render_gui_menu>();


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