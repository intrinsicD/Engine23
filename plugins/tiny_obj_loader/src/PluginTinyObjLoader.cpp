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
            Mesh mesh;
            mesh.vertices.positions.resize(attrib.vertices.size() / 3);
            mesh.vertices.normals.resize(attrib.normals.size() / 3);
            mesh.vertices.colors.resize(attrib.colors.size() / 3);

            Vec3<float> min = {attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]};
            Vec3<float> max = {attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]};
            for (size_t i = 0; i < attrib.vertices.size() / 3; i++) {
                mesh.vertices.positions[i].x = attrib.vertices[3 * i + 0];
                mesh.vertices.positions[i].y = attrib.vertices[3 * i + 1];
                mesh.vertices.positions[i].z = attrib.vertices[3 * i + 2];
                min.x = std::min(min.x, mesh.vertices.positions[i].x);
                min.y = std::min(min.y, mesh.vertices.positions[i].y);
                min.z = std::min(min.z, mesh.vertices.positions[i].z);
                max.x = std::max(max.x, mesh.vertices.positions[i].x);
                max.y = std::max(max.y, mesh.vertices.positions[i].y);
                max.z = std::max(max.z, mesh.vertices.positions[i].z);
            }

            for(auto &v: mesh.vertices.positions) {
                v.x = (v.x - min.x) / (max.x - min.x);
                v.y = (v.y - min.y) / (max.y - min.y);
                v.z = (v.z - min.z) / (max.z - min.z);
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
                    Vec3<unsigned int> vertices;
                    Vec3<unsigned int> normals;
                    Vec3<unsigned int> texcoords;
                    for (size_t v = 0; v < fv; v++) {
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        vertices.data[v] = idx.vertex_index;
                        normals.data[v] = idx.normal_index;
                        texcoords.data[v] = idx.texcoord_index;
                    }
                    if (vertices.data[0] != vertices.data[1] && vertices.data[0] != vertices.data[2] &&
                        vertices.data[1] != vertices.data[2]) {
                        mesh.faces.vertices.push_back(vertices);
                    }
                    if (normals.data[0] != normals.data[1] && normals.data[0] != normals.data[2] &&
                        normals.data[1] != normals.data[2]) {
                        mesh.faces.normals.push_back(normals);
                    }
                    if (texcoords.data[0] != texcoords.data[1] && texcoords.data[0] != texcoords.data[2] &&
                        texcoords.data[1] != texcoords.data[2]) {
                        mesh.faces.texcoords.push_back(texcoords);
                    }
                    index_offset += fv;
                }
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
            renderable_triangles.vbo.set_data(mesh.vertices.positions.data(), mesh.vertices.positions.size() * sizeof(mesh.vertices.positions[0]));

            renderable_triangles.ebo.bind();
            renderable_triangles.ebo.set_data(mesh.faces.vertices.data(), mesh.faces.vertices.size() * sizeof(mesh.faces.vertices[0]));

            renderable_triangles.vao.set_float_attribute(0, 3, false, (void *) 0);

            renderable_triangles.vbo.release();
            renderable_triangles.vao.release();
            renderable_triangles.ebo.release();
            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            renderable_triangles.program = programs["learn_opengl"];
            renderable_triangles.count = mesh.faces.vertices.size() * 3;
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
                Log::Warn("TinyObjLoader: Could not load any obj file").enqueue();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&on_update>();
        }


        void on_shutdown(const Events::Shutdown<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&on_update>();
        }
    }

    PluginTinyObjLoader::PluginTinyObjLoader() : Plugin("tiny_obj_loader") {

    }

    void PluginTinyObjLoader::pre_init() {

    }

    void PluginTinyObjLoader::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginTinyObjLoaderInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().connect<&PluginTinyObjLoaderInternal::on_shutdown>();
        Log::Info(m_name + ": Initialized").enqueue();
    }

    void PluginTinyObjLoader::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().disconnect<&PluginTinyObjLoaderInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().disconnect<&PluginTinyObjLoaderInternal::on_shutdown>();
        Log::Info(m_name + ": Removed").enqueue();
    }
}