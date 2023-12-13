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
            {
                auto view = Engine::State().view<OpenGL::RenderableTriangles>();
                for (const auto &entity_id: view) {
                    auto &renderable = view.get<OpenGL::RenderableTriangles>(entity_id);
                    renderable.program.use();
                    renderable.program.set_vec3("our_color", renderable.our_color);
                    renderable.vao.bind();
                    renderable.draw();
                    renderable.vao.release();
                }
            }
        }

        static bool show_gui = false;

        void on_render_gui(const Events::Render<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Render<Gui>>().disconnect<&PluginLearnOpenGLInternal::on_render_gui>();
                return;
            }

            static bool hello_triangle = false;
            static entt::entity hello_triangle_id = entt::null;
            if (ImGui::Begin("Learn OpenGL", &show_gui)) {
                if (ImGui::Checkbox("Hello Triangle", &hello_triangle)) {
                    if (hello_triangle) {
                        Mesh mesh;
                        mesh.vertices.positions = {{0.5f,  0.5f,  0.0f},
                                                   {0.5f,  -0.5f, 0.0f},
                                                   {-0.5f, -0.5f, 0.0f},
                                                   {-0.5f, 0.5f,  0.0f}};
                        mesh.faces.vertices = {{0, 1, 3},
                                               {1, 2, 3}};

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
                        auto &renderable_triangles = Engine::State().emplace<OpenGL::RenderableTriangles>(
                                hello_triangle_id);
                        renderable_triangles = OpenGL::RenderableTriangles::Create();
                        renderable_triangles.vbo = OpenGL::VertexBufferObject::Static();
                        renderable_triangles.ebo = OpenGL::IndexBufferObject::Static();

                        renderable_triangles.vao.create();
                        renderable_triangles.vbo.create();
                        renderable_triangles.ebo.create();

                        renderable_triangles.vao.bind();
                        renderable_triangles.vbo.bind();
                        //size = 48 = 3 * 4 * 4 = dims * num_vertices * sizeof(type)
                       // renderable_triangles.vbo.set_data(vertices.data(), vertices.size() * sizeof(vertices[0]));
                        renderable_triangles.vbo.set_data(mesh.vertices.positions.data(), mesh.vertices.positions.size() * mesh.vertices.positions[0].dims() * sizeof(mesh.vertices.positions[0].x));

                        renderable_triangles.ebo.bind();
                        //size = 24 = 3 * 2 * 4 = dims * num_triangles * sizeof(type)
                        //renderable_triangles.ebo.set_data(indices.data(), indices.size() * sizeof(indices[0]));
                        renderable_triangles.ebo.set_data(mesh.faces.vertices.data(), mesh.faces.vertices.size() * mesh.faces.vertices[0].dims() * sizeof(mesh.faces.vertices[0].x));
                        //dims = 3
                        //renderable_triangles.vao.set_float_attribute(0, 3, false, (void *) 0);
                        renderable_triangles.vao.set_float_attribute(0, mesh.vertices.positions[0].dims(), false, (void *) 0);

                        renderable_triangles.vbo.release();
                        renderable_triangles.vao.release();
                        renderable_triangles.ebo.release();
                        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                        renderable_triangles.program = programs["learn_opengl"];
                        //count = 6 = 3 * 2 = dims * num_triangles
                        //renderable_triangles.count = 6;
                        renderable_triangles.count = mesh.faces.vertices[0].dims() * mesh.faces.vertices.size();
                        renderable_triangles.offset = 0;
                        renderable_triangles.our_color[0] = 1.0f;
                        renderable_triangles.our_color[1] = 0.5f;
                        renderable_triangles.our_color[2] = 0.2f;
                    } else {
                        auto &renderable = Engine::State().get<OpenGL::RenderableTriangles>(hello_triangle_id);
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
                                      "uniform vec3 our_color;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   FragColor = vec4(our_color, 1.0f);\n"
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