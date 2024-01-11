//
// Created by alex on 07.12.23.
//

#include "PluginLearnOpenGL.h"
#include "OpenGLUtils.h"
#include "Engine.h"
#include "Events.h"
#include "Commands.h"
#include "imgui.h"
#include "glad/gl.h"
#include "glm/gtc/type_ptr.hpp"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/TriMesh.h"
#include "GlmUtils.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginLearnOpenGLInternal {
        static bool show_gui = false;

        void on_render_frame(const Events::Render<Frame> &event);

        void on_render_gui(const Events::Render<Gui> &event);

        void on_render_gui_menu(const Events::Render<GuiMenu> &event);

        void on_startup(const Events::Startup<Plugin> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace PluginLearnOpenGLInternal {
        void on_render_frame(const Events::Render<Frame> &event) {
/*            {
                auto &camera = Engine::Context().get<Camera>();
                auto view = Engine::State().view<OpenGL::RenderableTriangles, Transform>();
                for (const auto &entity_id: view) {
                    auto &renderable = view.get<OpenGL::RenderableTriangles>(entity_id);
                    auto &transform = view.get<Transform>(entity_id);
                    renderable.program.use();
                    renderable.program.set_vec3("our_color", renderable.our_color);
                    renderable.program.set_mat4("view", glm::value_ptr(camera.get_view()));
                    renderable.program.set_mat4("projection", glm::value_ptr(camera.get_projection()));
                    renderable.program.set_mat4("model", glm::value_ptr(transform.model));
                    renderable.vao.bind();
                    renderable.draw();
                    renderable.vao.release();
                }
            }*/
        }

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
                        TriMesh mesh;
                        mesh.vertices.positions = {{0.5f,  0.5f,  0.0f},
                                                   {0.5f,  -0.5f, 0.0f},
                                                   {-0.5f, -0.5f, 0.0f},
                                                   {-0.5f, 0.5f,  0.0f}};
                        mesh.faces.indices = {{0, 1, 3},
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
                        renderable_triangles.vbo.set_data(mesh.vertices.positions.data(),
                                                          mesh.vertices.positions.size() *
                                                          dims(mesh.vertices.positions[0]) *
                                                          sizeof(mesh.vertices.positions[0].x));

                        renderable_triangles.ebo.bind();
                        //size = 24 = 3 * 2 * 4 = dims * num_triangles * sizeof(type)
                        //renderable_triangles.ebo.set_data(indices.data(), indices.size() * sizeof(indices[0]));
                        renderable_triangles.ebo.set_data(mesh.faces.indices.data(),
                                                          mesh.faces.indices.size() * dims(mesh.faces.indices[0]) *
                                                          sizeof(mesh.faces.indices[0].x));
                        //dims = 3
                        //renderable_triangles.vao.set_float_attribute(0, 3, false, (void *) 0);
                        renderable_triangles.vao.set_float_attribute(0, dims(mesh.vertices.positions[0]), false,
                                                                     (void *) 0);

                        renderable_triangles.vbo.release();
                        renderable_triangles.vao.release();
                        renderable_triangles.ebo.release();
                        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                        renderable_triangles.program = programs["learn_opengl"];
                        //count = 6 = 3 * 2 = dims * num_triangles
                        //renderable_triangles.count = 6;
                        renderable_triangles.count = dims(mesh.faces.indices[0]) * mesh.faces.indices.size();
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

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            OpenGL::ShaderProgram program;
            program.v_shader.filepath = "../backend/glsl/programs/simple_shading/vertex_shader.glsl";
            program.f_shader.filepath = "../backend/glsl/programs/simple_shading/fragment_shader.glsl";
            program.name = "learn_opengl";
            program.load_shaders();
/*            program.v_shader.source = "#version 330 core\n"
                                      "layout (location = 0) in vec3 aPos;\n"
                                      "layout (location = 1) in vec3 aNormal;\n"
                                      "out vec3 Normal;\n"
                                      "uniform mat4 view;\n"
                                      "uniform mat4 projection;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   Normal = aNormal;\n"
                                      "   gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                      "}\0";
            program.f_shader.source = "#version 330 core\n"
                                      "in vec3 Normal;\n"
                                      "out vec4 FragColor;\n"
                                      "uniform vec3 our_color;\n"
                                      "void main()\n"
                                      "{\n"
                                      //"   FragColor = vec4(Normal * log(1 + gl_FragCoord.z), 1.0f);\n"
                                      "   FragColor = vec4(Normal * abs(Normal.z), 1.0f);\n"
                                      "}\n\0";*/
            program.compile_from_sources();
            program.link();
            if (program.check_link_status()) {
                programs[program.name] = program;
            } else {
                auto m_name = PluginLearnOpenGL().name();
                Log::Error(m_name + ": " + program.name + " Error: " + program.error_message).enqueue();
            }

            Log::Info(PluginLearnOpenGL().name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Render<Frame>>().disconnect<&on_render_frame>();

            Log::Info(PluginLearnOpenGL().name() + ": Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    PluginLearnOpenGL::PluginLearnOpenGL() : Plugin("learn_opengl") {

    }

    void PluginLearnOpenGL::pre_init() {

    }

    void PluginLearnOpenGL::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginLearnOpenGLInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&PluginLearnOpenGLInternal::on_shutdown>();

        Log::Info(name() + ": Initialized").enqueue();
    }

    void PluginLearnOpenGL::remove() {
        Log::Info(name() + ": Removed").enqueue();
    }
}