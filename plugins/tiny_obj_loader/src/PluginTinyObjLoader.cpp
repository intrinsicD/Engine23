//
// Created by alex on 11.12.23.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "PluginTinyObjLoader.h"
#include "Commands.h"
#include "Components.h"
#include "Events.h"
#include "Engine.h"
#include "fmt/core.h"
#include "OpenGLUtils.h"

namespace Bcg {
    namespace PluginTinyObjLoaderInternal {
        bool load_obj(const std::string &filepath) {
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = "./"; // Path to material files

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
            AABB aabb;
            mesh.vertices.positions.resize(attrib.vertices.size() / 3);
            mesh.vertices.normals.resize(attrib.normals.size() / 3);
            mesh.vertices.colors.resize(attrib.colors.size() / 3);

            glm::vec3 center = {0.0, 0.0, 0.0};
            for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
                mesh.vertices.positions[i].x = attrib.vertices[3 * i + 0];
                mesh.vertices.positions[i].y = attrib.vertices[3 * i + 1];
                mesh.vertices.positions[i].z = attrib.vertices[3 * i + 2];
                center.x += mesh.vertices.positions[i].x;
                center.y += mesh.vertices.positions[i].y;
                center.z += mesh.vertices.positions[i].z;

                aabb.grow(mesh.vertices.positions[i]);
            }
            auto abs_max = glm::max(aabb.max, -aabb.min);
            auto scaling = glm::max(abs_max.x, glm::max(abs_max.y, abs_max.z));
            center.x /= mesh.vertices.positions.size();
            center.y /= mesh.vertices.positions.size();
            center.z /= mesh.vertices.positions.size();

            for(auto &v: mesh.vertices.positions) {
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
                        mesh.faces.vertices.push_back(vertices);
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
            for (size_t i = 0; i < mesh.faces.vertices.size(); i++) {
                auto v0 = mesh.vertices.positions[mesh.faces.vertices[i][0]];
                auto v1 = mesh.vertices.positions[mesh.faces.vertices[i][1]];
                auto v2 = mesh.vertices.positions[mesh.faces.vertices[i][2]];
                auto normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                for (size_t j = 0; j < 3; j++) {
                    mesh.vertices.normals[mesh.faces.vertices[i][j]] += normal;
                }
            }

            for (size_t i = 0; i < mesh.vertices.normals.size(); i++) {
                mesh.vertices.normals[i] = glm::normalize(mesh.vertices.normals[i]);
            }

            entt::entity hello_triangle_id = Engine::State().create();
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
            renderable_triangles.vbo.set_data(nullptr, 2 * mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) * sizeof(mesh.vertices.positions[0].x));
            //renderable_triangles.vbo.set_data(mesh.vertices.positions.data(), mesh.vertices.positions.size() * mesh.vertices.positions[0].dims() * sizeof(mesh.vertices.positions[0].x));
            renderable_triangles.vbo.set_sub_data(mesh.vertices.positions.data(), mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) * sizeof(mesh.vertices.positions[0].x), 0);
            renderable_triangles.vbo.set_sub_data(mesh.vertices.normals.data(), mesh.vertices.normals.size() * dims(mesh.vertices.normals[0]) * sizeof(mesh.vertices.normals[0].x), mesh.vertices.positions.size() * dims(mesh.vertices.positions[0]) * sizeof(mesh.vertices.positions[0].x));

            renderable_triangles.ebo.bind();
            renderable_triangles.ebo.set_data(mesh.faces.vertices.data(), mesh.faces.vertices.size() * dims(mesh.faces.vertices[0]) * sizeof(mesh.faces.vertices[0].x));

            renderable_triangles.vao.set_float_attribute(0, dims(mesh.vertices.positions[0]), false, (void *) 0);
            renderable_triangles.vao.set_float_attribute(1, dims(mesh.vertices.normals[0]), false,
                                                         (void *) (mesh.vertices.positions.size() *
                                                                   dims(mesh.vertices.positions[0]) *
                                                                   sizeof(mesh.vertices.positions[0].x)));

            renderable_triangles.vbo.release();
            renderable_triangles.vao.release();
            renderable_triangles.ebo.release();
            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            renderable_triangles.program = programs["learn_opengl"];
            renderable_triangles.count = dims(mesh.faces.vertices[0]) * mesh.faces.vertices.size();
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
                    could_load_obj |= load_obj(path);
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