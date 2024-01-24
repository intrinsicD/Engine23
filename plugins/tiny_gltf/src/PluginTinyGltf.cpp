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

        struct Accessors : public std::vector<tinygltf::Accessor> {
            using std::vector<tinygltf::Accessor>::vector;
        };

        struct Animations : public std::vector<tinygltf::Animation> {
            using std::vector<tinygltf::Animation>::vector;
        };

        struct Buffers : public std::vector<tinygltf::Buffer> {
            using std::vector<tinygltf::Buffer>::vector;
        };

        struct BufferViews : public std::vector<tinygltf::BufferView> {
            using std::vector<tinygltf::BufferView>::vector;
        };

        struct Materials : public std::vector<tinygltf::Material> {
            using std::vector<tinygltf::Material>::vector;
        };

        struct Meshes : public std::vector<tinygltf::Mesh> {
            using std::vector<tinygltf::Mesh>::vector;
        };

        struct Nodes : public std::vector<tinygltf::Node> {
            using std::vector<tinygltf::Node>::vector;
        };

        struct Textures : public std::vector<tinygltf::Texture> {
            using std::vector<tinygltf::Texture>::vector;
        };

        struct Images : public std::vector<tinygltf::Image> {
            using std::vector<tinygltf::Image>::vector;
        };

        struct Skins : public std::vector<tinygltf::Skin> {
            using std::vector<tinygltf::Skin>::vector;
        };

        struct Samplers : public std::vector<tinygltf::Sampler> {
            using std::vector<tinygltf::Sampler>::vector;
        };

        struct Cameras : public std::vector<tinygltf::Camera> {
            using std::vector<tinygltf::Camera>::vector;
        };

        struct Scenes : public std::vector<tinygltf::Scene> {
            using std::vector<tinygltf::Scene>::vector;
        };

        struct Lights : public std::vector<tinygltf::Light> {
            using std::vector<tinygltf::Light>::vector;
        };

        struct AudioEmitters : public std::vector<tinygltf::AudioEmitter> {
            using std::vector<tinygltf::AudioEmitter>::vector;
        };

        struct AudioSources : public std::vector<tinygltf::AudioSource> {
            using std::vector<tinygltf::AudioSource>::vector;
        };

        struct Assets : public std::vector<tinygltf::Asset> {
            using std::vector<tinygltf::Asset>::vector;
        };

        struct Model {
            int default_scene = -1;
            int asset = -1;
            entt::entity entity = entt::null;
        };

        struct Models : public std::vector<Model> {
            using std::vector<Model>::vector;
        };
    }
}

namespace Bcg {
    namespace PluginTinyGltfInternal {
        void convert(tinygltf::Model &model, TriMesh &mesh) {
            const tinygltf::Mesh &tiny_mesh = model.meshes[0];
            const tinygltf::Primitive &primitive = tiny_mesh.primitives[0];

            const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

            mesh.faces.indices.resize(accessor.count);

            memcpy(mesh.faces.indices.data(), &buffer.data.at(accessor.byteOffset + bufferView.byteOffset),
                   accessor.count * sizeof(uint32_t));

            const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView &posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer &posBuffer = model.buffers[posBufferView.buffer];

            mesh.vertices.positions.resize(posAccessor.count * posAccessor.type);

            memcpy(mesh.vertices.positions.data(),
                   &posBuffer.data.at(posAccessor.byteOffset + posBufferView.byteOffset),
                   posAccessor.count * posAccessor.type * sizeof(float));

            // Check for normals
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                // The model has vertex normals
                const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[posBufferView.buffer];

                mesh.vertices.normals.resize(accessor.count * accessor.type);

                memcpy(mesh.vertices.normals.data(), &buffer.data.at(accessor.byteOffset + buffer_view.byteOffset),
                       accessor.count * accessor.type * sizeof(float));
            }

// Check for vertex colors
            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                // The model has vertex colors
                const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.at("COLOR_0")];
                const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[posBufferView.buffer];

                mesh.vertices.colors.resize(accessor.count * accessor.type);

                memcpy(mesh.vertices.colors.data(), &buffer.data.at(accessor.byteOffset + buffer_view.byteOffset),
                       accessor.count * accessor.type * sizeof(float));
            }
        }

        bool load_gltf(const std::string &filepath) {
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

            return ret;
        }

        bool load_glb(const std::string &filepath) {
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

            auto &models = Engine::Context().get<Models>();
            auto &assets = Engine::Context().get<Assets>();
            assets.emplace_back(model.asset);
            auto &my_model = models.emplace_back(
                    Model{model.defaultScene, static_cast<int>(assets.size() - 1), Engine::State().create()});
            auto &accessors = Engine::State().emplace<Accessors>(my_model.entity, model.accessors);
            auto &animations = Engine::State().emplace<Animations>(my_model.entity, model.animations);
            auto &buffers = Engine::State().emplace<Buffers>(my_model.entity, model.buffers);
            auto &buffer_views = Engine::State().emplace<BufferViews>(my_model.entity, model.bufferViews);
            auto &materials = Engine::State().emplace<Materials>(my_model.entity, model.materials);
            auto &meshes = Engine::State().emplace<Meshes>(my_model.entity, model.meshes);
            auto &nodes = Engine::State().emplace<Nodes>(my_model.entity, model.nodes);
            auto &textures = Engine::State().emplace<Textures>(my_model.entity, model.textures);
            auto &images = Engine::State().emplace<Images>(my_model.entity, model.images);
            auto &skins = Engine::State().emplace<Skins>(my_model.entity, model.skins);
            auto &samplers = Engine::State().emplace<Samplers>(my_model.entity, model.samplers);
            auto &cameras = Engine::State().emplace<Cameras>(my_model.entity, model.cameras);
            auto &scenes = Engine::State().emplace<Scenes>(my_model.entity, model.scenes);
            auto &lights = Engine::State().emplace<Lights>(my_model.entity, model.lights);
            auto &audio_emitters = Engine::State().emplace<AudioEmitters>(my_model.entity, model.audioEmitters);
            auto &audio_sources = Engine::State().emplace<AudioSources>(my_model.entity, model.audioSources);

            return ret;
        }

        void on_update(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            bool could_load = false;
            for (auto &path: input.drop.paths) {
                if (path.find(".gltf") != std::string::npos) {
                    could_load |= load_gltf(path);
                } else if (path.find(".glb") != std::string::npos) {
                    could_load |= load_glb(path);
                }
            }

            if (!could_load) {
                Log::Warn(PluginTinyGltf().name() + ": Could not load any gltf or glb file").enqueue();
            }
        }

        void on_startup(const Events::Startup<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&on_update>();

            Engine::Context().emplace<Models>();
            Engine::Context().emplace<Assets>();

            Log::Info(PluginTinyGltf().name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Plugin> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&on_update>();
            Log::Info(PluginTinyGltf().name() + ": Shutdown").enqueue();
        }
    }
}

namespace Bcg {
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