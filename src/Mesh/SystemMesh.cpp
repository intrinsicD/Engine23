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
#include "Entity.h"
#include "SystemTransform.h"
#include "SystemAABB.h"
#include "SystemAsset.h"
#include "Components.h"
#include "ImGuiUtils.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemMeshInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

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
                if(ImGui::BeginMenu(SystemMesh::component_name().c_str())){
                    if (ImGui::MenuItem("Instance", nullptr, &show_gui_instance)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_instance>();
                    }
                    if (ImGui::MenuItem("Components", nullptr, &show_gui_components)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_components>();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui_instance(const Events::Update<Gui> &event) {
            if (!show_gui_instance) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemMesh::component_name().c_str(), &show_gui_instance)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Mesh>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("MeshComponents", &show_gui_components)) {
                Components<Mesh> components(SystemMesh::component_name());
                ImGuiUtils::Show(components);
            }
            ImGui::End();
        }

        void on_update_input_drop(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            for (const auto &path: input.drop.paths) {
                //Check if loader for extension exists
                if (!MeshIo::has_file_extension(FilePath::Extension(path))) {
                    continue;
                }
                entt::entity entity_id = entt::null;
                Engine::Dispatcher().trigger(Events::Create<entt::entity>{&entity_id});
                Engine::Dispatcher().trigger(Events::Load<Mesh>{entity_id, path});
                CompositeCommand prepare_entity("prepare_entity");
                prepare_entity.add_task("compute_vertex_normals", [entity_id]() {
                    Components<Mesh> components(SystemMesh::component_name());
                    auto &mesh = components.get_instance(entity_id);

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
                        Components<Mesh> components(SystemMesh::component_name());
                        auto &mesh = components.get_instance(entity_id);

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
        Components<Mesh> components(SystemMesh::component_name());
        auto instance_id = components.create_instance();
        components.add_to_entity(entity_id, instance_id);

        auto &mesh = components.get_instance(instance_id);

        if (!reader.read(mesh)) {
            Log::Warn(component_name() + ": load from file FAILED: " + filepath).enqueue();
            return false;
        } else {
            Log::Info(component_name() + ": loaded from file: " + filepath).enqueue();
        }

        Components<Asset> assets(SystemAsset::component_name());
        auto asset_id = assets.create_instance();
        assets.add_to_entity(entity_id, asset_id);
        assets.get_instance(asset_id) = Asset(FilePath::Filename(filepath), filepath, component_name());

        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
        auto aabb = AABB3();
        aabb.fit(MapConst(positions));

        auto scaling = aabb.max.cwiseMax(-aabb.min).maxCoeff();

        auto center = aabb.center();
        for (auto v: mesh.vertices) {
            positions[v] = (positions[v] - center) / scaling;
        }

        Components<Transform<float>> transforms(SystemTransform::component_name());
        auto transform_id = transforms.create_instance();
        transforms.add_to_entity(entity_id, transform_id);

        Components<AABB3> aabbs(SystemAABB::component_name());
        auto aabb_id = aabbs.create_instance();
        aabbs.add_to_entity(entity_id, aabb_id);
        aabbs.get_instance(aabb_id).fit(MapConst(positions));

        return true;
    }

    void SystemMesh::pre_init() {

    }

    void SystemMesh::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemMeshInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemMeshInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemMeshInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Update<Input::Drop>>().connect<&SystemMeshInternal::on_update_input_drop>();
        Engine::Dispatcher().sink<Events::Load<Mesh>>().connect<&SystemMeshInternal::on_load_mesh>();
        Engine::Dispatcher().sink<Events::RemoveAll<Component<Mesh>>>().connect<&SystemMeshInternal::on_remove_all>();

        Log::Info("Initialized", name()).enqueue();
    }

    void SystemMesh::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemMeshInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemMeshInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemMeshInternal::on_update_gui_menu>();
        Engine::Dispatcher().sink<Events::Update<Input::Drop>>().disconnect<&SystemMeshInternal::on_update_input_drop>();
        Engine::Dispatcher().sink<Events::Load<Mesh>>().disconnect<&SystemMeshInternal::on_load_mesh>();
        Engine::Dispatcher().sink<Events::RemoveAll<Component<Mesh>>>().disconnect<&SystemMeshInternal::on_remove_all>();

        Log::Info("Removed", name()).enqueue();
    }
}