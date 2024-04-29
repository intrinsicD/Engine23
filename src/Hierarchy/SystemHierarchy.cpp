//
// Created by alex on 19.01.24.
//

#include "SystemHierarchy.h"
#include "SystemsUtils.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "Hierarchy.h"
#include "HierarchyGui.h"
#include "Picker.h"
#include "imgui.h"

namespace Bcg {
    namespace SystemHierarchyInternal {
        static bool show_gui = false;

        void on_update_gui(const Events::Update<Gui> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}


namespace Bcg {
    namespace SystemHierarchyInternal {
        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("Hierarchy", &show_gui)) {
                auto &picker = Engine::Context().get<Picker>();
                ComponentGui<Hierarchy>::Show(picker.id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Entity")) {
                if (ImGui::MenuItem("Hierarchy", nullptr, &show_gui)) {
                    Engine::Instance()->dispatcher.sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }


        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<GuiMenu>>().connect<&SystemHierarchyInternal::on_update_gui_menu>();
            Engine::State().on_construct<Hierarchy>().connect<&on_construct_component<SystemHierarchy>>();
            Engine::State().on_update<Hierarchy>().connect<&on_update_component<SystemHierarchy>>();
            Engine::State().on_destroy<Hierarchy>().connect<&on_destroy_component<SystemHierarchy>>();
            Log::Info(SystemHierarchy::name() , "Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Update<GuiMenu>>().disconnect<&SystemHierarchyInternal::on_update_gui_menu>();
            Engine::State().on_construct<Hierarchy>().disconnect<&on_construct_component<SystemHierarchy>>();
            Engine::State().on_update<Hierarchy>().disconnect<&on_update_component<SystemHierarchy>>();
            Engine::State().on_destroy<Hierarchy>().disconnect<&on_destroy_component<SystemHierarchy>>();
            Log::Info(SystemHierarchy::name() , "Shutdown").enqueue();
        }
    }
}


namespace Bcg {
    bool SystemHierarchy::has_component(entt::entity &entity_id) {
        return Engine::State().all_of<Hierarchy>(entity_id);
    }

    Hierarchy &SystemHierarchy::get_or_add(entt::entity entity_id) {
        return Engine::State().get_or_emplace<Hierarchy>(entity_id);
    }

    bool SystemHierarchy::destroy(entt::entity entity_id) {
        auto *hierarchy = Engine::State().try_get<Hierarchy>(entity_id);
        if (hierarchy) {
            auto children = get_children(entity_id);
            for (auto child_id: children) {
                auto &child_hierarchy = Engine::State().get<Hierarchy>(child_id);
                child_hierarchy.parent = hierarchy->parent;
            }

            if (hierarchy->parent != entt::null) {
                auto &parent_hierarchy = Engine::State().get<Hierarchy>(hierarchy->parent);
                auto &last_child_hierarchy = Engine::State().get<Hierarchy>(children.back());
                last_child_hierarchy.next = parent_hierarchy.first;
                parent_hierarchy.first = children.front();
            }

            Engine::State().remove<Hierarchy>(entity_id);
            return true;
        }
        return false;
    }

    bool SystemHierarchy::detach_from_parent(entt::entity entity_id) {
        auto *hierarchy = Engine::State().try_get<Hierarchy>(entity_id);
        if (!hierarchy) return false;
        auto *parent_hierarchy = Engine::State().try_get<Hierarchy>(hierarchy->parent);
        if (!parent_hierarchy) return false;

        auto *prev_hierarchy = Engine::State().try_get<Hierarchy>(hierarchy->prev);
        auto *next_hierarchy = Engine::State().try_get<Hierarchy>(hierarchy->next);

        //remove from the list of siblings (prev case)
        if (prev_hierarchy) {
            prev_hierarchy->next = hierarchy->next;
        }

        //remove from the list of siblings (next case)
        if (next_hierarchy) {
            next_hierarchy->prev = hierarchy->prev;
        }

        //replace first child
        if (parent_hierarchy->first == entity_id) {
            parent_hierarchy->first = hierarchy->next;
            assert(!prev_hierarchy);
        } else {
            assert(prev_hierarchy);
        }

        hierarchy->prev = entt::null;
        hierarchy->next = entt::null;

        hierarchy->parent = entt::null;

        assert(parent_hierarchy->children > 0);
        --parent_hierarchy->children;
        return true;
    }

    void SystemHierarchy::attach_child(entt::entity parent_id, entt::entity child_id) {
        auto *parent_hierarchy = Engine::State().try_get<Hierarchy>(parent_id);
        auto *child_hierarchy = Engine::State().try_get<Hierarchy>(child_id);

        if (!parent_hierarchy) {
            parent_hierarchy = &Engine::State().emplace<Hierarchy>(parent_id);
        }

        auto *first_child_hierarchy = Engine::State().try_get<Hierarchy>(parent_hierarchy->first);

        if (!child_hierarchy) {
            child_hierarchy = &Engine::State().emplace<Hierarchy>(child_id);
        } else {
            detach_from_parent(child_id);
        }

        child_hierarchy->parent = parent_id;

        if (first_child_hierarchy) {
            first_child_hierarchy->prev = child_id;
            child_hierarchy->next = parent_hierarchy->first;
        }

        parent_hierarchy->first = child_id;
        ++parent_hierarchy->children;
    }

    std::vector<entt::entity> SystemHierarchy::get_children(entt::entity entity_id) {
        auto *hierarchy = Engine::State().try_get<Hierarchy>(entity_id);
        if (!hierarchy) {
            Log::Error("Entity's Hierarchy-Component is missing. This should not happen!");
            return {};
        }
        entt::entity current = hierarchy->first;
        std::vector<entt::entity> children;
        while (current != entt::null) {
            children.push_back(current);
            auto *current_child = Engine::State().try_get<Hierarchy>(current);
            if (!current_child) {
                Log::Error("Child's Hierarchy-Component is missing. This should not happen!");
                break;
            }
            current = current_child->next;
        }
        return children;
    }

    entt::entity SystemHierarchy::get_root(entt::entity entity_id) {
        auto *hierarchy = Engine::State().try_get<Hierarchy>(entity_id);
        if (!hierarchy) {
            Log::Error("Entity's Hierarchy-Component is missing. This should not happen!");
            return entt::null;
        }
        if (hierarchy->parent == entt::null) return entity_id;
        return get_root(hierarchy->parent);
    }

    std::string SystemHierarchy::name() {
        return "SystemHierarchy";
    }

    std::string SystemHierarchy::component_name() {
        return "Hierarchy";
    }

    void SystemHierarchy::pre_init() {

    }

    void SystemHierarchy::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemHierarchyInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemHierarchyInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemHierarchy::remove() {
        Log::Info("Removed", name()).enqueue();
    }
}