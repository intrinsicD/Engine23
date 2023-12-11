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

namespace Bcg{
    namespace PluginTinyObjLoaderInternal{
        bool load_obj(const std::string &filepath){
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

            auto& attrib = reader.GetAttrib();
            auto& shapes = reader.GetShapes();
            auto& materials = reader.GetMaterials();


            Log::Info(fmt::format("# of vertices  = {}", attrib.vertices.size() / 3)).enqueue();
            Log::Info(fmt::format("# of normals   = {}", attrib.normals.size() / 3)).enqueue();
            Log::Info(fmt::format("# of texcoords = {}", attrib.texcoords.size() / 2)).enqueue();
            Log::Info(fmt::format("# of shapes    = {}", shapes.size())).enqueue();
            Log::Info(fmt::format("# of materials = {}", materials.size())).enqueue();


// Loop over shapes
            for (size_t s = 0; s < shapes.size(); s++) {
                // Loop over faces(polygon)
                size_t index_offset = 0;
                for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                    size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                    // Loop over vertices in the face.
                    for (size_t v = 0; v < fv; v++) {
                        // access to vertex
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                        tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                        tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                        // Check if `normal_index` is zero or positive. negative = no normal data
                        if (idx.normal_index >= 0) {
                            tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                            tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                            tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                        }

                        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                        if (idx.texcoord_index >= 0) {
                            tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                            tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                        }

                        // Optional: vertex colors
                        // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                        // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                        // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                    }
                    index_offset += fv;

                    // per-face material
                    shapes[s].mesh.material_ids[f];
                }
            }
            return true;
        }

        void on_update(const Events::Update<Input::Drop> &event){
            auto &input = Engine::Context().get<Input>();

            bool could_load_obj = false;
            for (auto &path : input.drop.paths) {
                if(path.find(".obj") != std::string::npos){
                    could_load_obj |= load_obj(path);
                }
            }

            if(!could_load_obj){
                Log::Warn("TinyObjLoader: Could not load any obj file").enqueue();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event){
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&on_update>();
        }


        void on_shutdown(const Events::Shutdown<Plugin> &event){
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