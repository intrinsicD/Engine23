//
// Created by alex on 18.06.24.
//

#include "CommandsMaterial.h"
#include "Engine.h"
#include "ActiveMaterials.h"
#include "Attribute.h"
#include "Material.h"
#include "Components.h"

namespace Bcg {
    InitMaterials::InitMaterials(entt::entity entity_id) : AbstractCommand("InitMaterials"), entity_id(entity_id) {

    }

    int InitMaterials::execute() {
        auto entity = Entity(Engine::State(), entity_id);
        Components<ActiveMaterials> active_materials;
        unsigned int active_material_id = -1;
        if (!entity.all_of<Component<ActiveMaterials>>()) {
            active_material_id = active_materials.create_instance();
            entity.emplace(Component<ActiveMaterials>{active_material_id});
            Log::Info("Added Component<ActiveMaterials> to entity " + AsString(entity_id)).execute();
        } else {
            auto &component_active_materials = entity.get<Component<ActiveMaterials>>();
            active_material_id = component_active_materials.index;
        }

        auto &active_material = active_materials.get_instance(active_material_id);
        Components<Material> materials;
        if (entity.vertices() && active_material.point_cloud_material_idx == -1) {
            active_material.point_cloud_material_idx = materials.create_instance();
            auto &material = materials.get_instance(active_material.point_cloud_material_idx);
            material.name = "Point Cloud Material";
            Log::Info("Added Material " + material.name + " to entity " + AsString(entity_id)).execute();
        }

        if (entity.edges() && active_material.graph_material_idx == -1) {
            active_material.graph_material_idx = materials.create_instance();
            auto &material = materials.get_instance(active_material.graph_material_idx);
            material.name = "Graph Material";
            Log::Info("Added Material " + material.name + " to entity " + AsString(entity_id)).execute();
        }

        if (entity.faces() && active_material.mesh_material_idx == -1) {
            active_material.mesh_material_idx = materials.create_instance();
            auto &material = materials.get_instance(active_material.mesh_material_idx);
            material.name = "Mesh Material";
            Log::Info("Added Material " + material.name + " to entity " + AsString(entity_id)).execute();
        }
        return 1;
    }
}