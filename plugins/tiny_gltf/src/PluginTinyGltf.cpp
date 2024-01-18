//
// Created by alex on 18.01.24.
//

#include "PluginTinyGltf.h"
#include "Events.h"
#include "Engine.h"
#include "Commands.h"
#include "components/Input.h"
#include "components/TriMesh.h"
#include "FilePath.h"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

namespace Bcg {
    namespace PluginTinyGltfInternal {
        void convert(tinygltf::Model &model, TriMesh &mesh);

        bool load_gltf(const std::string &filepath);

        bool load_glb(const std::string &filepath);

        void on_update(const Events::Update<Input::Drop> &event);

        void on_startup(const Events::Startup<Plugin> &event);

        void on_shutdown(const Events::Shutdown<Plugin> &event);
    }
}

namespace Bcg {
    namespace PluginTinyGltfInternal {
        void convert(tinygltf::Model &model, TriMesh &mesh){
            const tinygltf::Mesh& tiny_mesh = model.meshes[0];
            const tinygltf::Primitive& primitive = tiny_mesh.primitives[0];

            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            mesh.faces.indices.resize(accessor.count);

            memcpy(mesh.faces.indices.data(), &buffer.data.at(accessor.byteOffset + bufferView.byteOffset),
                   accessor.count * sizeof(uint32_t));

            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

            mesh.vertices.positions.resize(posAccessor.count * posAccessor.type);

            memcpy(mesh.vertices.positions.data(), &posBuffer.data.at(posAccessor.byteOffset + posBufferView.byteOffset),
                   posAccessor.count * posAccessor.type * sizeof(float));

            // Check for normals
            if(primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                // The model has vertex normals
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[posBufferView.buffer];

                mesh.vertices.normals.resize(accessor.count * accessor.type);

                memcpy(mesh.vertices.normals.data(), &buffer.data.at(accessor.byteOffset + buffer_view.byteOffset),
                       accessor.count * accessor.type * sizeof(float));
            }

// Check for vertex colors
            if(primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                // The model has vertex colors
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("COLOR_0")];
                const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[posBufferView.buffer];

                mesh.vertices.colors.resize(accessor.count * accessor.type);

                memcpy(mesh.vertices.colors.data(), &buffer.data.at(accessor.byteOffset + buffer_view.byteOffset),
                       accessor.count * accessor.type * sizeof(float));
            }
        }

        bool load_gltf(const std::string &filepath){
            tinygltf::Model model;
            tinygltf::TinyGLTF loader;
            std::string err;
            std::string warn;

            bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);

            if (!warn.empty()) {
                printf("Warn: %s\n", warn.c_str());
            }

            if (!err.empty()) {
                printf("Err: %s\n", err.c_str());
            }

            if (!ret) {
                printf("Failed to parse glTF\n");
                return false;
            }


        }

        bool load_glb(const std::string &filepath){
            tinygltf::Model model;
            tinygltf::TinyGLTF loader;
            std::string err;
            std::string warn;

            bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath); // for binary glTF(.glb)

            if (!warn.empty()) {
                printf("Warn: %s\n", warn.c_str());
            }

            if (!err.empty()) {
                printf("Err: %s\n", err.c_str());
            }

            if (!ret) {
                printf("Failed to parse glTF\n");
                return false;
            }
        }

        void on_update(const Events::Update<Input::Drop> &event){
            auto &input = Engine::Context().get<Input>();

            bool could_load = false;
            for (auto &path: input.drop.paths) {
                if (path.find(".gltf") != std::string::npos) {
                    could_load |= load_gltf(path);
                }else if(path.find(".glb") != std::string::npos) {
                    could_load |= load_glb(path);
                }
            }

            if (!could_load) {
                Log::Warn(PluginTinyGltf().name() + ": Could not load any gltf or glb file").enqueue();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event){
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&on_update>();
            Log::Info(PluginTinyGltf().name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Plugin> &event){
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&on_update>();
            Log::Info(PluginTinyGltf().name() + ": Shutdown").enqueue();
        }
    }
}

namespace Bcg{
    PluginTinyGltf::PluginTinyGltf() : Plugin("tiny_gltf") {}

    void PluginTinyGltf::pre_init() {

    }

    void PluginTinyGltf::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().connect<&PluginTinyGltfInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().connect<&PluginTinyGltfInternal::on_shutdown>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void PluginTinyGltf::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Plugin>>().disconnect<&PluginTinyGltfInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Plugin>>().disconnect<&PluginTinyGltfInternal::on_shutdown>();
        Log::Info(name() + ": Removed").enqueue();
    }
}