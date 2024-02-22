//
// Created by alex on 11.12.23.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "PluginTinyObjLoader.h"
#include "Commands.h"
#include "Events.h"
#include "Engine.h"
#include "fmt/core.h"
#include "OpenGLUtils.h"
#include "GlmUtils.h"
#include "glm/gtc/type_ptr.hpp"
#include "components/Input.h"
#include "AABB.h"
#include "Mesh.h"
#include "Eigen/Geometry"
#include "PropertyEigenMap.h"
#include "components/Transform.h"
#include "components/EntityName.h"
#include "components/TriMesh.h"
#include "components/Camera.h"
#include "FilePath.h"

namespace Bcg {
    namespace PluginTinyObjLoaderInternal {
        bool load_obj_glm(const std::string &filepath);

        bool load_obj_eigen(const std::string &filepath);

        void on_update(const Events::Update<Input::Drop> &event);

        void on_startup(const Events::Startup<Plugin> &event);

        void on_shutdown(const Events::Shutdown<Plugin> &event);
    }
}

namespace Bcg {
    namespace PluginTinyObjLoaderInternal {
        bool load_obj_glm(const std::string &filepath) {
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = FilePath::Parent(filepath);

            tinyobj::ObjReader reader;

            if (!reader.ParseFromFile(filepath, reader_config)) {
                if (!reader.Error().empty()) {
                    Log::Error(fmt::format("TinyObjReader: {}", reader.Error())).enqueue();
                }
                return false;
            }

            if (!reader.Warning().empty()) {
                Log::Warn(fmt::format("TinyObjReader: {}", reader.Warning())).enqueue();
            }

            auto &attrib = reader.GetAttrib();
            auto &shapes = reader.GetShapes();
            auto &materials = reader.GetMaterials();

            Log::Info(fmt::format("# of vertices  = {}", attrib.vertices.size() / 3)).enqueue();
            Log::Info(fmt::format("# of normals   = {}", attrib.normals.size() / 3)).enqueue();
            Log::Info(fmt::format("# of texcoords = {}", attrib.texcoords.size() / 2)).enqueue();
            Log::Info(fmt::format("# of shapes    = {}", shapes.size())).enqueue();
            Log::Info(fmt::format("# of materials = {}", materials.size())).enqueue();
            //print number of faces
            for (size_t s = 0; s < shapes.size(); s++) {
                Log::Info(fmt::format("# of faces in shape[{}] = {}", s,
                                      shapes[s].mesh.num_face_vertices.size())).enqueue();
            }

            //convert to mesh
            TriMesh mesh;
            AABB3 aabb;
            mesh.vertices.positions.resize(attrib.vertices.size() / 3);
            mesh.vertices.normals.resize(attrib.normals.size() / 3);
            mesh.vertices.colors.resize(attrib.colors.size() / 3);

            glm::vec3 center = {0.0, 0.0, 0.0};
            for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
                auto x = attrib.vertices[3 * i + 0];
                auto y = attrib.vertices[3 * i + 1];
                auto z = attrib.vertices[3 * i + 2];
                mesh.vertices.positions[i].x = x;
                mesh.vertices.positions[i].y = y;
                mesh.vertices.positions[i].z = z;
                center.x += x;
                center.y += y;
                center.z += z;

                aabb.grow(Eigen::Vector<double, 3>(x, y, z));
                Log::Info(fmt::format("v[{}]: ({}, {}, {})", i, x, y, z)).enqueue();
            }


            auto abs_max = aabb.max.cwiseMax(-aabb.min);
            auto scaling = glm::max(abs_max[0], glm::max(abs_max[1], abs_max[2]));
            center.x /= mesh.vertices.positions.size();
            center.y /= mesh.vertices.positions.size();
            center.z /= mesh.vertices.positions.size();

            for (auto &v: mesh.vertices.positions) {
                v.x = (v.x - center.x) / scaling;
                v.y = (v.y - center.y) / scaling;
                v.z = (v.z - center.z) / scaling;
            }

            for (size_t i = 0; i < attrib.normals.size() / 3; i++) {
                mesh.vertices.normals[i].x = attrib.normals[3 * i + 0];
                mesh.vertices.normals[i].y = attrib.normals[3 * i + 1];
                mesh.vertices.normals[i].z = attrib.normals[3 * i + 2];
            }

            for (size_t i = 0; i < attrib.colors.size() / 3; i++) {
                mesh.vertices.colors[i].x = attrib.colors[3 * i + 0];
                mesh.vertices.colors[i].y = attrib.colors[3 * i + 1];
                mesh.vertices.colors[i].z = attrib.colors[3 * i + 2];
            }

            //fill triangle index list for the faces, such that each face is a triangle
            for (size_t s = 0; s < shapes.size(); s++) {
                size_t index_offset = 0;
                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                    size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                    glm::uvec3 vertices;
                    glm::uvec3 normals;
                    glm::uvec3 texcoords;
                    for (size_t v = 0; v < fv; v++) {
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        vertices[v] = idx.vertex_index;
                        normals[v] = idx.normal_index;
                        texcoords[v] = idx.texcoord_index;
                    }
                    if (vertices[0] != vertices[1] && vertices[0] != vertices[2] &&
                        vertices[1] != vertices[2]) {
                        mesh.faces.indices.push_back(vertices);
                        Log::Info(
                                fmt::format("f[{}]: ({}, {}, {})", f, vertices[0], vertices[1], vertices[2])).enqueue();
                    }
                    if (normals[0] != normals[1] && normals[0] != normals[2] &&
                        normals[1] != normals[2]) {
                        mesh.faces.normals.push_back(normals);
                    }
                    if (texcoords[0] != texcoords[1] && texcoords[0] != texcoords[2] &&
                        texcoords[1] != texcoords[2]) {
                        mesh.faces.texcoords.push_back(texcoords);
                    }
                    index_offset += fv;
                }
            }

            //compute the normals from the vertices and faces
            mesh.vertices.normals.resize(mesh.vertices.positions.size());
            for (size_t i = 0; i < mesh.faces.indices.size(); i++) {
                auto v0 = mesh.vertices.positions[mesh.faces.indices[i][0]];
                auto v1 = mesh.vertices.positions[mesh.faces.indices[i][1]];
                auto v2 = mesh.vertices.positions[mesh.faces.indices[i][2]];
                auto normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                for (size_t j = 0; j < 3; j++) {
                    mesh.vertices.normals[mesh.faces.indices[i][j]] += normal;
                }
            }

            for (size_t i = 0; i < mesh.vertices.normals.size(); i++) {
                mesh.vertices.normals[i] = glm::normalize(mesh.vertices.normals[i]);
            }

            entt::entity entity_id;
            Engine::Instance()->dispatcher.trigger(Events::Create<entt::entity>{&entity_id});
            auto &renderable_triangles = Engine::State().emplace<OpenGL::RenderableTriangles>(entity_id);
            Engine::State().emplace<AABB3>(entity_id, aabb);
            Engine::State().emplace<Transform>(entity_id);
            Engine::State().emplace<EntityName>(entity_id, filepath);
            Engine::State().emplace<TriMesh>(entity_id, mesh);
            RenderCommand render_command;
            render_command.add_command_sptr(std::make_shared<TaskCommand>("forward render", [entity_id]() {
                auto &camera = Engine::Context().get<Camera>();
                auto &renderable = Engine::State().get<OpenGL::RenderableTriangles>(entity_id);
                auto &transform = Engine::State().get<Transform>(entity_id);
                renderable.program.use();
                renderable.program.set_vec3("our_color", renderable.our_color);
                renderable.program.set_mat4("view", glm::value_ptr(camera.get_view()));
                renderable.program.set_mat4("projection", glm::value_ptr(camera.get_projection()));
                renderable.program.set_mat4("model", glm::value_ptr(transform.model));
                renderable.vao.bind();
                renderable.draw();
                renderable.vao.release();
                return 1;
            }));
            Engine::State().emplace<RenderCommand>(entity_id, render_command);


            renderable_triangles = OpenGL::RenderableTriangles::Create();
            renderable_triangles.vbo = OpenGL::VertexBufferObject::Static();
            renderable_triangles.ebo = OpenGL::IndexBufferObject::Static();

            renderable_triangles.vao.create();
            renderable_triangles.vbo.create();
            renderable_triangles.ebo.create();

            renderable_triangles.vao.bind();
            renderable_triangles.vbo.bind();
            Log::Info(fmt::format("pos size_in_bytes: {}",
                                  mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) *
                                  sizeof(mesh.vertices.positions[0].x))).enqueue();
            renderable_triangles.vbo.set_data(nullptr,
                                              2 * mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) *
                                              sizeof(mesh.vertices.positions[0].x));
            //renderable_triangles.vbo.set_data(mesh.vertices.positions.data(), mesh.vertices.positions.size() * mesh.vertices.positions[0].dims() * sizeof(mesh.vertices.positions[0].x));
            renderable_triangles.vbo.set_sub_data(mesh.vertices.positions.data(),
                                                  mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) *
                                                  sizeof(mesh.vertices.positions[0].x), 0);
            renderable_triangles.vbo.set_sub_data(mesh.vertices.normals.data(),
                                                  mesh.vertices.normals.size() * dims(mesh.vertices.normals[0]) *
                                                  sizeof(mesh.vertices.normals[0].x),
                                                  mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) *
                                                  sizeof(mesh.vertices.positions[0].x));

            renderable_triangles.ebo.bind();
            renderable_triangles.ebo.set_data(mesh.faces.indices.data(),
                                              mesh.faces.indices.size() * dims(mesh.faces.indices[0]) *
                                              sizeof(mesh.faces.indices[0].x));
            Log::Info(fmt::format("tris size_in_bytes: {}", mesh.faces.indices.size() * dims(mesh.faces.indices[0]) *
                                                            sizeof(mesh.faces.indices[0].x))).enqueue();
            renderable_triangles.vao.set_float_attribute(0, dims(mesh.vertices.positions[0]), false, (void *) 0);
            renderable_triangles.vao.set_float_attribute(1, dims(mesh.vertices.normals[0]), false,
                                                         (void *) (mesh.vertices.normals.size() *
                                                                   dims(mesh.vertices.normals[0]) *
                                                                   sizeof(mesh.vertices.normals[0].x)));
            renderable_triangles.vao.set_float_attribute(2, dims(mesh.vertices.normals[0]), false,
                                                         (void *) (mesh.vertices.normals.size() *
                                                                   dims(mesh.vertices.normals[0]) *
                                                                   sizeof(mesh.vertices.normals[0].x)));

            renderable_triangles.vbo.release();
            renderable_triangles.vao.release();
            renderable_triangles.ebo.release();
            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            renderable_triangles.program = programs["learn_opengl"];
            renderable_triangles.count = dims(mesh.faces.indices[0]) * mesh.faces.indices.size();
            renderable_triangles.offset = 0;
            renderable_triangles.our_color[0] = 1.0f;
            renderable_triangles.our_color[1] = 0.5f;
            renderable_triangles.our_color[2] = 0.2f;
            return true;
        }

        bool load_obj_eigen(const std::string &filepath) {
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = FilePath::Parent(filepath);

            tinyobj::ObjReader reader;

            if (!reader.ParseFromFile(filepath, reader_config)) {
                if (!reader.Error().empty()) {
                    Log::Error(fmt::format("TinyObjReader: {}", reader.Error())).enqueue();
                }
                return false;
            }

            if (!reader.Warning().empty()) {
                Log::Warn(fmt::format("TinyObjReader: {}", reader.Warning())).enqueue();
            }

            auto &attrib = reader.GetAttrib();
            auto &shapes = reader.GetShapes();
            auto &materials = reader.GetMaterials();

            Log::Info(fmt::format("# of vertices  = {}", attrib.vertices.size() / 3)).enqueue();
            Log::Info(fmt::format("# of normals   = {}", attrib.normals.size() / 3)).enqueue();
            Log::Info(fmt::format("# of texcoords = {}", attrib.texcoords.size() / 2)).enqueue();
            Log::Info(fmt::format("# of shapes    = {}", shapes.size())).enqueue();
            Log::Info(fmt::format("# of materials = {}", materials.size())).enqueue();
            //print number of faces
            for (size_t s = 0; s < shapes.size(); s++) {
                Log::Info(fmt::format("# of faces in shape[{}] = {}", s,
                                      shapes[s].mesh.num_face_vertices.size())).enqueue();
            }

            //convert to mesh
            Mesh mesh;
            auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_position");
            auto normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_normal");
            auto colors = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_color");
            AABB3 aabb;
            mesh.vertices.reserve(attrib.vertices.size() / 3);

            for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
                auto point = Eigen::Vector<double, 3>(attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1],
                                                      attrib.vertices[3 * i + 2]);
                mesh.add_vertex(point, positions);

                aabb.grow(point);
            }

            auto scaling = aabb.max.cwiseMax(-aabb.min).maxCoeff();

            auto center = aabb.center();
            for (auto v: mesh.vertices) {
                positions[v] = (positions[v] - center) / scaling;
            }

            for (size_t i = 0; i < attrib.normals.size() / 3; i++) {
                normals[i] = Eigen::Vector<double, 3>(attrib.normals[3 * i + 0], attrib.normals[3 * i + 1],
                                                      attrib.normals[3 * i + 2]);
            }

            for (size_t i = 0; i < attrib.colors.size() / 3; i++) {
                colors[i] = Eigen::Vector<double, 3>(attrib.colors[3 * i + 0], attrib.colors[3 * i + 1],
                                                     attrib.colors[3 * i + 2]);
            }

            //fill triangle index list for the faces, such that each face is a triangle
            for (size_t s = 0; s < shapes.size(); s++) {
                size_t index_offset = 0;
                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                    size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                    glm::uvec3 vertices;
/*                    glm::uvec3 normals;
                    glm::uvec3 texcoords;*/
                    for (size_t v = 0; v < fv; v++) {
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        vertices[v] = idx.vertex_index;
                        /*                    normals[v] = idx.normal_index;
                                            texcoords[v] = idx.texcoord_index;*/
                    }
                    if (vertices[0] != vertices[1] && vertices[0] != vertices[2] &&
                        vertices[1] != vertices[2]) {
                        mesh.add_face({vertices[0], vertices[1], vertices[2]});
                    }
/*                    if (normals[0] != normals[1] && normals[0] != normals[2] &&
                        normals[1] != normals[2]) {
                        mesh.faces.normals.push_back(normals);
                    }
                    if (texcoords[0] != texcoords[1] && texcoords[0] != texcoords[2] &&
                        texcoords[1] != texcoords[2]) {
                        mesh.faces.texcoords.push_back(texcoords);
                    }*/
                    index_offset += fv;
                }
            }

            //compute the normals from the vertices and faces
            auto triangles = mesh.get_triangles();
            for (auto f: mesh.faces) {
                auto v0 = positions[triangles[f][0]];
                auto v1 = positions[triangles[f][1]];
                auto v2 = positions[triangles[f][2]];
                auto normal = (v1 - v0).cross(v2 - v0).normalized();

                for (size_t v = 0; v < 3; v++) {
                    normals[triangles[f][v]] += normal;
                }
            }

            for (auto v: mesh.vertices) {
                normals[v].normalize();
            }
            Log::Info(fmt::format("{}\n{}", positions.info(), positions.to_string())).enqueue();
            Log::Info(fmt::format("{}\n{}", triangles.info(), triangles.to_string())).enqueue();

            entt::entity entity_id;
            Engine::Instance()->dispatcher.trigger(Events::Create<entt::entity>{&entity_id});
            auto &renderable_triangles = Engine::State().emplace<OpenGL::RenderableTriangles>(entity_id);
            Engine::State().emplace<AABB3>(entity_id, aabb);
            Engine::State().emplace<Transform>(entity_id);
            Engine::State().emplace<EntityName>(entity_id, filepath);
            Engine::State().emplace<Mesh>(entity_id, mesh);
            RenderCommand render_command;
            render_command.add_command_sptr(std::make_shared<TaskCommand>("forward render", [entity_id]() {
                auto &camera = Engine::Context().get<Camera>();
                auto &renderable = Engine::State().get<OpenGL::RenderableTriangles>(entity_id);
                auto &transform = Engine::State().get<Transform>(entity_id);
                renderable.program.use();
                renderable.program.set_vec3("our_color", renderable.our_color);
                renderable.program.set_mat4("view", glm::value_ptr(camera.get_view()));
                renderable.program.set_mat4("projection", glm::value_ptr(camera.get_projection()));
                renderable.program.set_mat4("model", glm::value_ptr(transform.model));
                renderable.vao.bind();
                renderable.draw();
                renderable.vao.release();
                return 1;
            }));
            Engine::State().emplace<RenderCommand>(entity_id, render_command);


            renderable_triangles = OpenGL::RenderableTriangles::Create();
            renderable_triangles.vbo = OpenGL::VertexBufferObject::Static();
            renderable_triangles.ebo = OpenGL::IndexBufferObject::Static();

            renderable_triangles.vao.create();
            renderable_triangles.vbo.create();
            renderable_triangles.ebo.create();

            renderable_triangles.vao.bind();
            renderable_triangles.vbo.bind();
            Eigen::Matrix<float, 3, -1> positions_float = Map(positions).transpose().cast<float>();
            Eigen::Matrix<float, 3, -1> normals_float = Map(normals).transpose().cast<float>();
            unsigned int size_in_bytes = positions_float.size() * sizeof(float);
            renderable_triangles.vbo.set_data(nullptr, 2 * size_in_bytes);
            Log::Info(fmt::format("pos size_in_bytes: {}", size_in_bytes)).enqueue();

            renderable_triangles.vbo.set_sub_data(positions_float.data(), size_in_bytes, 0);
            renderable_triangles.vbo.set_sub_data(normals_float.data(), size_in_bytes, size_in_bytes);

/*            renderable_triangles.vbo.set_data(nullptr, 2 * positions.get_size_bytes());
            Log::Info(fmt::format("pos size_in_bytes: {}", positions.get_size_bytes())).enqueue();

            renderable_triangles.vbo.set_sub_data(positions.get_void_ptr(), positions.get_size_bytes(), 0);
            renderable_triangles.vbo.set_sub_data(normals.get_void_ptr(), normals.get_size_bytes(),
                                                  normals.get_size_bytes());*/

            renderable_triangles.ebo.bind();
            renderable_triangles.ebo.set_data(triangles.get_void_ptr(), triangles.get_size_bytes());

            Log::Info(fmt::format("tris size_in_bytes: {}", triangles.get_size_bytes())).enqueue();

            renderable_triangles.vao.set_float_attribute(0, positions.get_dims(), false, (void *) 0);
            renderable_triangles.vao.set_float_attribute(1, normals.get_dims(), false, (void *) (size_in_bytes));
            renderable_triangles.vao.set_float_attribute(2, normals.get_dims(), false, (void *) (size_in_bytes));

            renderable_triangles.vbo.release();
            renderable_triangles.vao.release();
            renderable_triangles.ebo.release();
            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            renderable_triangles.program = programs["learn_opengl"];
            renderable_triangles.count = triangles.get_dims() * triangles.get_size();
            renderable_triangles.offset = 0;
            renderable_triangles.our_color[0] = 1.0f;
            renderable_triangles.our_color[1] = 0.5f;
            renderable_triangles.our_color[2] = 0.2f;
            return true;
        }

        void on_update(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            bool could_load_obj = false;
            for (auto &path: input.drop.paths) {
                if (path.find(".obj") != std::string::npos) {
                    //could_load_obj |= load_obj_glm(path);
                    could_load_obj |= load_obj_eigen(path);
                }
            }

            if (!could_load_obj) {
                Log::Warn(PluginTinyObjLoader().name() + ": Could not load any obj file").enqueue();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&on_update>();
            Log::Info(PluginTinyObjLoader().name() + ": Startup").enqueue();
        }


        void on_shutdown(const Events::Shutdown<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&on_update>();
            Log::Info(PluginTinyObjLoader().name() + ": Shutdown").enqueue();
        }
    }
}

namespace Bcg {
    PluginTinyObjLoader::PluginTinyObjLoader() : Plugin("tiny_obj_loader") {

    }

    void PluginTinyObjLoader::pre_init() {

    }

    void PluginTinyObjLoader::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginTinyObjLoaderInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().connect<&PluginTinyObjLoaderInternal::on_shutdown>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void PluginTinyObjLoader::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().disconnect<&PluginTinyObjLoaderInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().disconnect<&PluginTinyObjLoaderInternal::on_shutdown>();
        Log::Info(name() + ": Removed").enqueue();
    }
}