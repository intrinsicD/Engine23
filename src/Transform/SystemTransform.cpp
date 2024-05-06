//
// Created by alex on 05.01.24.
//

#include "Transform/SystemTransform.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "Transform.h"
#include "TransformGui.h"
#include "Picker.h"
#include "Entity.h"
#include "imgui.h"
#include "Component.h"
#include "ResourceContainer.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    using ResourceContainerTransform = ResourceContainer<Transform<float>>;

    namespace SystemTransformInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTransformInternal {

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemTransformInternal::on_update_gui>();
                return;
            }

            if (ImGui::Begin("Transform", &show_gui, ImGuiWindowFlags_AlwaysAutoResize)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Transform<float>>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {

                if (ImGui::MenuItem("Transform", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&SystemTransformInternal::on_update_gui>();
                }

                ImGui::EndMenu();
            }
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemTransformInternal::on_update_gui_menu>();
            Log::Info(SystemTransform::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemTransformInternal::on_update_gui_menu>();
            Log::Info(SystemTransform::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemTransform::name() {
        return "System" + component_name();
    }

    std::string SystemTransform::component_name() {
        return "Transform";
    }

    unsigned int SystemTransform::create_instance() {
        auto &instances = Engine::Context().get<ResourceContainerTransform>();
        if (!instances.free_list.empty()) {
            unsigned int instance_id = instances.free_list.back();
            instances.free_list.pop_back();
            instances.pool[instance_id].model.setIdentity();
            Log::Info("Reuse " + component_name() + " instance with instance_id: " +
                      std::to_string(instance_id)).enqueue();
            return instance_id;
        } else {
            unsigned int instance_id = instances.get_size();
            instances.push_back();
            Log::Info("Created " + component_name() + " instance with instance_id: " +
                      std::to_string(instance_id)).enqueue();
            return instance_id;
        }
    }

    void SystemTransform::destroy_instance(unsigned int instance_id) {
        auto &instances = Engine::Context().get<ResourceContainerTransform>();
        instances.free_list.push_back(instance_id);
        Log::Info(
                "Destroy " + component_name() + " instance with instance_id: " + std::to_string(instance_id)).enqueue();
    }

    void SystemTransform::add_to_entity(entt::entity entity_id, unsigned int instance_id) {
        Engine::State().emplace_or_replace<Component<Transform<float>>>(entity_id, instance_id);
        Log::Info(
                "Add " + component_name() + " with instance_id: " + std::to_string(instance_id) + " to entity_id: " +
                AsString(entity_id)).enqueue();
    }

    void SystemTransform::remove_from_entity(entt::entity entity_id) {
        Engine::State().remove<Component<Transform<float>>>(entity_id);
        Log::Info("Removed " + component_name() + " from entity_id: " + AsString(entity_id)).enqueue();
    }

    void SystemTransform::pre_init() {
        Engine::Context().emplace<ResourceContainerTransform>();
    }

    void SystemTransform::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemTransformInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemTransformInternal::on_shutdown_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemTransform::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemTransformInternal::on_startup_engine>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemTransformInternal::on_shutdown_engine>();
        Engine::Context().erase<ResourceContainerTransform>();
        Log::Info("Removed", name()).enqueue();
    }
}