//
// Created by alex on 22.02.24.
//

#include "SystemMesh.h"
#include "SystemsUtils.h"
#include "Engine.h"
#include "Events/Events.h"
#include "imgui.h"
#include "FilePath.h"
#include "Asset.h"
#include "AABB.h"
#include "PropertyEigenMap.h"
#include "MeshGui.h"
#include "MeshIo.h"
#include "Transform.h"
#include "OpenGLUtils.h"
#include "GLMeshRenderPass.h"
#include "MeshVertexNormalsAreaAngleHybrid.h"
#include "CommandDoubleBuffer.h"
#include "Input.h"
#include "Picker.h"
#include "ResourceContainer.h"
#include "Component.h"
#include "Entity.h"
#include "SystemTransform.h"
#include "SystemAABB.h"
#include "SystemAsset.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    using ResourceContainerMesh = ResourceContainer<Mesh>;
            
    namespace SystemMeshInternal {
        static bool show_gui = false;
        static bool show_gui_resource_container = false;

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_resource_container(const Events::Update<Gui> &event);

        void on_update_input_drop(const Events::Update<Input::Drop> &event);

        void on_load_mesh(const Events::Load<Mesh> &event);

        void on_remove_all(const Events::RemoveAll<Component<Mesh>> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMeshInternal {
        void on_startup(const Events::Startup<Engine> &event) {
            Engine::State().on_construct<Mesh>().connect<&on_construct_component<SystemMesh>>();
            Engine::State().on_update<Mesh>().connect<&on_update_component<SystemMesh>>();
            Engine::State().on_destroy<Mesh>().connect<&on_destroy_component<SystemMesh>>();
            Log::Info(SystemMesh::name(), "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::State().on_construct<Mesh>().disconnect<&on_construct_component<SystemMesh>>();
            Engine::State().on_update<Mesh>().disconnect<&on_update_component<SystemMesh>>();
            Engine::State().on_destroy<Mesh>().disconnect<&on_destroy_component<SystemMesh>>();
            Log::Info(SystemMesh::name(), "Shutdown").enqueue();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Mesh", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                if (ImGui::MenuItem("MeshResourceContainer", nullptr, &show_gui_resource_container)) {
                    Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().connect<&on_update_gui_resource_container>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Mesh", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                auto entity_id = picker.id.entity;

                if (entity_id != entt::null && Engine::State().all_of<Component<Mesh>>(entity_id)) {
                    auto &instances = Engine::Context().get<ResourceContainerMesh>();
                    auto instance_id = Engine::State().get<Component<Mesh>>(entity_id).index;
                    auto &mesh = instances.pool[instance_id];
                    ComponentGui<Mesh>::Show(mesh);
                }
            }
            ImGui::End();
        }

        void on_update_gui_resource_container(const Events::Update<Gui> &event) {
            if (!show_gui_resource_container) {
                Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().disconnect<&on_update_gui_resource_container>();
                return;
            }

            if (ImGui::Begin("MeshResourceContainer", &show_gui_resource_container)) {
                auto &instances = Engine::Context().get<ResourceContainerMesh>();
                for (size_t instance_id = 0; instance_id < instances.get_size(); ++instance_id) {
                    if (ImGui::TreeNode(std::to_string(instance_id).c_str())) {
                        ComponentGui<Mesh>::Show(instances.pool[instance_id]);
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::End();
        }

        void on_update_input_drop(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            for (const auto &path: input.drop.paths) {
                entt::entity entity_id = entt::null;
                Engine::Dispatcher().trigger(Events::Create<entt::entity>{&entity_id});
                Engine::Dispatcher().trigger(Events::Load<Mesh>{entity_id, path});
                CompositeCommand prepare_entity("prepare_entity");
                prepare_entity.add_task("compute_vertex_normals", [entity_id]() {
                    auto &instances = Engine::Context().get<ResourceContainerMesh>();
                    auto instance_id = Engine::State().get<Component<Mesh>>(entity_id).index;
                    auto &mesh = instances.pool[instance_id];

                    auto normals = mesh.vertices.get<Eigen::Vector<double, 3>>("v_normal");
                    if (!normals) {
                        normals = MeshVertexNormalsAreaAngleHybrid(mesh);
                        Log::Info(fmt::format("Entity {} computed vertex normals!",
                                              static_cast<unsigned int>(entity_id))).enqueue();
                    } else {
                        Log::Info(fmt::format("Entity {} already has vertex normals!",
                                              static_cast<unsigned int>(entity_id))).enqueue();
                    }
                    return 1;
                });

                prepare_entity.add_task("upload_to_gpu", [entity_id]() {
                    if (!Engine::State().all_of<OpenGL::RenderableTriangles>(entity_id)) {
                        auto &instances = Engine::Context().get<ResourceContainerMesh>();
                        auto instance_id = Engine::State().get<Component<Mesh>>(entity_id).index;
                        auto &mesh = instances.pool[instance_id];

                        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
                        auto normals = mesh.vertices.get<Eigen::Vector<double, 3>>("v_normal");
                        auto triangles = mesh.get_triangles();

                        if (!normals) {
                            normals = MeshVertexNormalsAreaAngleHybrid(mesh);
                        }

                        auto renderable_triangles = OpenGL::RenderableTriangles::Create();
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

                        renderable_triangles.vbo.set_sub_data(positions_float.data(), size_in_bytes, 0);
                        renderable_triangles.vbo.set_sub_data(normals_float.data(), size_in_bytes, size_in_bytes);

                        renderable_triangles.ebo.bind();
                        renderable_triangles.ebo.set_data(triangles.get_void_ptr(), triangles.get_size_bytes());

                        renderable_triangles.vao.set_float_attribute(0, positions.get_dims(), false, (void *) 0);
                        renderable_triangles.vao.set_float_attribute(1, normals.get_dims(), false,
                                                                     (void *) (size_in_bytes));
                        renderable_triangles.vao.set_float_attribute(2, normals.get_dims(), false,
                                                                     (void *) (size_in_bytes));

                        renderable_triangles.vbo.release();
                        renderable_triangles.vao.release();
                        renderable_triangles.ebo.release();
                        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                        renderable_triangles.program = programs["simple_shading"];
                        renderable_triangles.count = triangles.get_dims() * triangles.get_size();
                        renderable_triangles.offset = 0;
                        renderable_triangles.our_color[0] = 1.0f;
                        renderable_triangles.our_color[1] = 0.5f;
                        renderable_triangles.our_color[2] = 0.2f;

                        Engine::State().emplace<OpenGL::RenderableTriangles>(entity_id, renderable_triangles);
                    }
                    return 1;
                });
                auto &command_buffer = Engine::Context().get<CommandDoubleBuffer>();
                command_buffer.enqueue_next(std::make_shared<CompositeCommand>(prepare_entity));
            }
        }

        void on_load_mesh(const Events::Load<Mesh> &event) {
            entt::entity entity_id = event.entity_id;
            if (entity_id == entt::null) {
                entity_id = Engine::State().create();
            }

            if (SystemMesh::load(event.filepath, entity_id)) {
                Engine::State().emplace_or_replace<GLMeshRenderPass>(entity_id);
            }
        }

        void on_remove_all(const Events::RemoveAll<Component<Mesh>> &event) {
            auto view = Engine::State().view<Component<Mesh>>();
            for (auto &entity_id: view) {
                Engine::State().erase<Component<Mesh>>(entity_id);
            }
            Log::Info("Removed all Component<" + SystemMesh::component_name() + ">").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemMesh::name() {
        return "System" + component_name();
    }

    std::string SystemMesh::component_name() {
        return "Mesh";
    }

    bool SystemMesh::load(const std::string &filepath, entt::entity entity_id) {
        MeshIo reader(filepath);
        auto instance_id = create_instance();
        add_to_entity(entity_id, instance_id);
        auto &instances = Engine::Context().get<ResourceContainerMesh>();
        auto &mesh = instances.pool[instance_id];

        if (!reader.read(mesh)) {
            Log::Warn("Failed to load mesh from file: " + filepath).enqueue();
            return false;
        } else {
            Log::Info("Loaded mesh from file: " + filepath).enqueue();
        }

        auto asset_id = SystemAsset::create_instance();
        SystemAsset::add_to_entity(entity_id, asset_id);
        auto &assets = Engine::Context().get<ResourceContainer<Asset>>();
        auto &asset = assets.pool[asset_id];

        asset.name = FilePath::Filename(filepath);
        asset.filepath = filepath;
        asset.type = "Mesh";

        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
        auto aabb = AABB3();
        aabb.fit(MapConst(positions));

        auto scaling = aabb.max.cwiseMax(-aabb.min).maxCoeff();

        auto center = aabb.center();
        for (auto v: mesh.vertices) {
            positions[v] = (positions[v] - center) / scaling;
        }

        auto transform_id = SystemTransform::create_instance();
        SystemTransform::add_to_entity(entity_id, transform_id);

        auto aabb_id = SystemAABB::create_instance();
        SystemAABB::add_to_entity(entity_id, aabb_id);

        auto &aabbs = Engine::Context().get<ResourceContainer<AABB3>>();
        aabbs.pool[aabb_id].fit(MapConst(positions));

        return true;
    }

    unsigned int SystemMesh::create_instance() {
        auto &instances = Engine::Context().get<ResourceContainerMesh>();
        if (!instances.free_list.empty()) {
            unsigned int instance_id = instances.free_list.back();
            instances.free_list.pop_back();
            instances.pool[instance_id] = Mesh();
            Log::Info("Reuse Mesh instance with instance_id: " + std::to_string(instance_id)).enqueue();
            return instance_id;
        } else {
            unsigned int instance_id = instances.get_size();
            instances.push_back();
            Log::Info("Created Mesh instance with instance_id: " + std::to_string(instance_id)).enqueue();
            return instance_id;
        }
    }

    void SystemMesh::destroy_instance(unsigned int instance_id) {
        auto &instances = Engine::Context().get<ResourceContainerMesh>();
        instances.free_list.push_back(instance_id);
        Log::Info("Destroy Mesh instance with instance_id: " + std::to_string(instance_id)).enqueue();
    }

    void SystemMesh::add_to_entity(entt::entity entity_id, unsigned int instance_id) {
        Engine::State().emplace_or_replace<Component<Mesh>>(entity_id, instance_id);
        Log::Info("Add Component<Mesh> with instance_id: " + std::to_string(instance_id) + " to entity_id: " +
                  AsString(entity_id)).enqueue();
    }

    void SystemMesh::remove_from_entity(entt::entity entity_id) {
        Engine::State().remove<Component<Mesh>>(entity_id);
        Log::Info("Removed Component<Mesh> from entity_id: " + AsString(entity_id)).enqueue();
    }

    void SystemMesh::pre_init() {
        Engine::Context().emplace<ResourceContainerMesh>();
    }

    void SystemMesh::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemMeshInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemMeshInternal::on_shutdown>();
        Engine::Instance()->dispatcher.sink<Events::Update<GuiMenu>>().connect<&SystemMeshInternal::on_update_gui_menu>();
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().connect<&SystemMeshInternal::on_update_input_drop>();
        Engine::Instance()->dispatcher.sink<Events::Load<Mesh>>().connect<&SystemMeshInternal::on_load_mesh>();
        Engine::Instance()->dispatcher.sink<Events::RemoveAll<Component<Mesh>>>().connect<&SystemMeshInternal::on_remove_all>();

        Log::Info("Initialized", name()).enqueue();
    }

    void SystemMesh::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemMeshInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemMeshInternal::on_shutdown>();
        Engine::Instance()->dispatcher.sink<Events::Update<GuiMenu>>().disconnect<&SystemMeshInternal::on_update_gui_menu>();
        Engine::Instance()->dispatcher.sink<Events::Update<Input::Drop>>().disconnect<&SystemMeshInternal::on_update_input_drop>();
        Engine::Instance()->dispatcher.sink<Events::Load<Mesh>>().disconnect<&SystemMeshInternal::on_load_mesh>();
        Engine::Instance()->dispatcher.sink<Events::RemoveAll<Component<Mesh>>>().disconnect<&SystemMeshInternal::on_remove_all>();

        Engine::Context().erase<ResourceContainerMesh>();
        Log::Info("Removed", name()).enqueue();
    }
}