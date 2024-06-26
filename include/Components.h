//
// Created by alex on 07.05.24.
//

#ifndef ENGINE23_COMPONENTS_H
#define ENGINE23_COMPONENTS_H

#include "Component.h"
#include "Resources/ResourceContainer.h"
#include "Engine.h"
#include "Commands.h"
#include "Entity.h"
#include "TypeName.h"
#include <string>

namespace Bcg {
    template<typename T>
    class Components {
    public:
        explicit Components(std::string name) : name(std::move(name)),
                                                container(Engine::Context().find<ResourceContainer<T>>()
                                                          ? Engine::Context().get<ResourceContainer<T>>()
                                                          : Engine::Context().emplace<ResourceContainer<T>>()) {

        }

        Components() : Components(TypeName<T>::name()) {

        }

        unsigned int create_instance() {
            if (!container.free_list.empty()) {
                unsigned int instance_id = container.free_list.back();
                container.free_list.pop_back();
                container.pool[instance_id] = T();
                Log::Info(name + ": reuse instance with instance_id: " + std::to_string(instance_id)).enqueue();
                container.used_list.emplace(instance_id);
                return instance_id;
            } else {
                unsigned int instance_id = container.get_size();
                container.used_list.emplace(instance_id);
                container.push_back();
                Log::Info(name + ": create instance with instance_id: " + std::to_string(instance_id)).enqueue();
                return instance_id;
            }
        }

        void destroy_instance(unsigned int instance_id) {
            if (instance_id < container.get_size()) {
                container.used_list.erase(instance_id);
                container.free_list.push_back(instance_id);
                Log::Info(name + ": destroy instance with instance_id: " + std::to_string(instance_id)).enqueue();
            } else {
                Log::Error(name + ": destroy instance with instance_id: " + std::to_string(instance_id) +
                           " failed, because instance_id is larger than the number of instances!").enqueue();

            }
        }

        T &get_instance(unsigned int instance_id) {
            return container.pool[instance_id];
        }

        T &get_instance(const Component<T> &component) {
            return get_instance(component.index);
        }

        T &get_instance(entt::entity entity_id) {
            return get_instance(Engine::State().get<Component<T>>(entity_id));
        }

        Component<T> &add_to_entity(entt::entity entity_id, unsigned int instance_id) {
            if (instance_id >= get_size()) {
                Log::Error(name + ": add with instance_id: " + std::to_string(instance_id) + " to entity_id: " +
                           AsString(entity_id) +
                           " failed, because instance_id is larger than the number of instances!").execute();
                throw std::runtime_error("instance_id is larger than the number of instances!");
            } else {
                Log::Info(name + ": add with instance_id: " + std::to_string(instance_id) + " to entity_id: " +
                          AsString(entity_id)).enqueue();
                return Engine::State().emplace_or_replace<Component<T>>(entity_id, instance_id);
            }
        }

        void remove_from_entity(entt::entity entity_id) {
            if (Engine::State().remove<Component<T>>(entity_id)) {
                Log::Info(name + ": remove from entity_id: " + AsString(entity_id)).enqueue();
            } else {
                Log::Error(name + ": remove from entity_id: " + AsString(entity_id) +
                           " failed, because entity_id does not have a " + name + " component!").enqueue();
            }
        }

        size_t get_size() {
            return container.get_size();
        }

        ResourceContainer<T> &container;
        std::string name;
    };
}

#endif //ENGINE23_COMPONENTS_H
